#include "InterThreadCommunicationMgr.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <algorithm>
#include <TinyLogger/LogInterface.h>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Renderer;
using namespace TinyLogger;

#define flipBits(n, b) ((n) ^ ((1u << (b)) - 1))

namespace Thread
{
   InterThreadCommunicationMgr::InterThreadCommunicationMgr()
       : mRenderThreadSwapChain()
   {
      LogInfo("InterThreadCommunicationMgr::ctor");
   }

   InterThreadCommunicationMgr::~InterThreadCommunicationMgr()
   {
      LogInfo("InterThreadCommunicationMgr::dctor");
   }

   void InterThreadCommunicationMgr::SetSceneRendererWP(std::weak_ptr<DeferredShadingSceneRenderer> sceneRenderer)
   {
      mSceneRenderer = sceneRenderer;
   }

   void InterThreadCommunicationMgr::SetSceneWP(std::weak_ptr<Scene> scene)
   {
      mScene = scene;
   }

   void InterThreadCommunicationMgr::SetLuaScriptProcessorWP(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor)
   {
      mLuaScriptProcessor = scriptProcessor;
   }

   std::weak_ptr<DeferredShadingSceneRenderer> InterThreadCommunicationMgr::GetSceneRendererWP() const
   {
      return mSceneRenderer;
   }

   std::weak_ptr<Scene> InterThreadCommunicationMgr::GetSceneWP() const
   {
      return mScene;
   }

   std::weak_ptr<LuaScriptProcessor> InterThreadCommunicationMgr::GetLuaScriptProcessor() const
   {
      return mLuaScriptProcessor;
   }

   void InterThreadCommunicationMgr::ExecuteOnRenderThread(eEnqueueJobPolicy policy, const int32_t creatorObjectId, const uint64_t functionId, std::function<void(void)> gameThreadJobCallback)
   {
      ProcessPushRenderThreadJob(policy, Job(creatorObjectId, functionId, gameThreadJobCallback));
   }

   void InterThreadCommunicationMgr::ExecuteOnGameThread(eEnqueueJobPolicy policy, const int32_t creatorObjectId, const uint64_t functionId, std::function<void(void)> renderThreadJobCallback)
   {
      ProcessPushGameThreadJob(policy, Job(creatorObjectId, functionId, renderThreadJobCallback));
   }

   void InterThreadCommunicationMgr::ExecuteOnLuaThread(eEnqueueJobPolicy policy, const int32_t creatorObjectId, const uint64_t functionId, std::function<void(void)> luaThreadJobCallback)
   {
      ProcessPushLuaThreadJob(policy, Job(creatorObjectId, functionId, luaThreadJobCallback));
   }

   void InterThreadCommunicationMgr::ProcessPushRenderThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      std::lock_guard<std::mutex> lock(mRenderThreadSwapChain.StoreOperationMutex);
      ProcessPushJob(policy, std::move(job), mRenderThreadSwapChain.GetDequeByIndex(mRenderThreadSwapChain.WriteChainType));
   }

   void InterThreadCommunicationMgr::ProcessPushGameThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      if (mIsAllowedPushGameThreadJobs.load(std::memory_order::memory_order_seq_cst))
      {
         std::lock_guard<std::mutex> lock(m_gameThreadMutex);
         ProcessPushJob(policy, std::move(job), m_gameThreadJobs);
      }
   }

   void InterThreadCommunicationMgr::ProcessPushLuaThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      if (mIsAllowedPushLuaThreadJobs.load(std::memory_order::memory_order_seq_cst))
      {
         std::lock_guard<std::mutex> lock(m_luaThreadMutex);
         ProcessPushJob(policy, std::move(job), m_luaThreadJobs);
      }
   }

   void InterThreadCommunicationMgr::ProcessPushJob(const eEnqueueJobPolicy policy, Job &&job, std::deque<Job> &jobs)
   {
      switch (policy)
      {
      case eEnqueueJobPolicy::PUSH_ANYWAY:
      {
         jobs.emplace_back(std::move(job));
         break;
      }
      case eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH:
      {
         const auto duplicateIt = std::find_if(jobs.begin(), jobs.end(),
                                               [&](const Job &collectionJob)
                                               {
                                                  return (collectionJob.GetCreatorObjectId() == job.GetCreatorObjectId() && collectionJob.GetFunctionId() == job.GetFunctionId());
                                               });

         if (jobs.end() == duplicateIt)
         {
            jobs.emplace_back(std::move(job));
         }

         break;
      }
      case eEnqueueJobPolicy::IF_DUPLICATE_REPLACE:
      {
         const auto duplicateIt = std::find_if(jobs.begin(), jobs.end(),
                                               [&](const Job &collectionJob)
                                               {
                                                  return ((collectionJob.GetCreatorObjectId() == job.GetCreatorObjectId()) &&
                                                          (collectionJob.GetFunctionId() == job.GetFunctionId()));
                                               });

         if (jobs.end() == duplicateIt)
         {
            jobs.emplace_back(std::move(job));
         }
         else
         {
            *(duplicateIt) = std::move(job);
         }

         break;
      }
      }
   }

   void InterThreadCommunicationMgr::SpinGameThreadJobs()
   {
      std::lock_guard<std::mutex> lock(m_gameThreadMutex);
      auto countGameThreadJobs = m_gameThreadJobs.size();
      while (countGameThreadJobs)
      {
         auto jobIt = m_gameThreadJobs.begin();
         (*jobIt)();
         m_gameThreadJobs.pop_front();
         --countGameThreadJobs;
      }
   }

   void InterThreadCommunicationMgr::SpinRenderThreadJobs()
   {
      auto &renderThreadChain = mRenderThreadSwapChain.GetDequeByIndex(mRenderThreadSwapChain.ReadChainType);
      auto countRenderThreadJobs = renderThreadChain.size();

      while (countRenderThreadJobs)
      {
         auto jobIt = renderThreadChain.begin();
         (*jobIt)();
         renderThreadChain.pop_front();
         --countRenderThreadJobs;
      }

      SwapRenderThreadChain();
   }

   void InterThreadCommunicationMgr::SpinLuaThreadJob()
   {
      std::lock_guard<std::mutex> lock(m_luaThreadMutex);
      auto countLuaThreadJobs = m_luaThreadJobs.size();
      while (countLuaThreadJobs)
      {
         auto jobIt = m_luaThreadJobs.begin();
         (*jobIt)();
         m_luaThreadJobs.pop_front();
         --countLuaThreadJobs;
      }
   }

   void InterThreadCommunicationMgr::SwapRenderThreadChain()
   {
      std::lock_guard<std::mutex> lock(mRenderThreadSwapChain.StoreOperationMutex);
      mRenderThreadSwapChain.ReadChainType = flipBits(mRenderThreadSwapChain.ReadChainType, 1);
      mRenderThreadSwapChain.WriteChainType = flipBits(mRenderThreadSwapChain.WriteChainType, 1);
   }

   void InterThreadCommunicationMgr::ClearGameThreadJobs()
   {
      std::lock_guard<std::mutex> lock(m_gameThreadMutex);
      m_gameThreadJobs.clear();
   }

   void InterThreadCommunicationMgr::ClearLuaThreadJobs()
   {
      std::lock_guard<std::mutex> lock(m_luaThreadMutex);
      m_luaThreadJobs.clear();
   }

   void InterThreadCommunicationMgr::SetIsAllowedPushGameThreadJobs(const bool isAllowed)
   {
      mIsAllowedPushGameThreadJobs.store(isAllowed, std::memory_order::memory_order_seq_cst);
   }

   void InterThreadCommunicationMgr::SetIsAllowedPushLuaThreadJobs(const bool isAllowed)
   {
      mIsAllowedPushLuaThreadJobs.store(isAllowed, std::memory_order::memory_order_seq_cst);
   }
}
#undef flipBits

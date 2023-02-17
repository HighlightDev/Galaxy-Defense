#include "InterThreadCommunicationMgr.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <algorithm>
#include <TinyLogger/LogInterface.h>

using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace TinyLogger;

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

   std::weak_ptr<DeferredShadingSceneRenderer> InterThreadCommunicationMgr::GetSceneRendererWP() const
   {
      return mSceneRenderer;
   }

   std::weak_ptr<Scene> InterThreadCommunicationMgr::GetSceneWP() const
   {
      return mScene;
   }

   void InterThreadCommunicationMgr::EmplaceGameThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      ProcessPushGameThreadJob(policy, std::move(job));
   }

   void InterThreadCommunicationMgr::EmplaceRenderThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      ProcessPushRenderThreadJob(policy, std::move(job));
   }

   void InterThreadCommunicationMgr::EmplaceLuaThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      ProcessPushLuaThreadJob(policy, std::move(job));
   }

   void InterThreadCommunicationMgr::ProcessPushRenderThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      std::lock_guard<std::mutex> lock(mRenderThreadSwapChain.StoreOperationMutex);
      ProcessPushJob(policy, std::move(job), mRenderThreadSwapChain.GetDequeByIndex(uint8_t(mRenderThreadSwapChain.WriteChainType.load())));
   }

   void InterThreadCommunicationMgr::ProcessPushGameThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      std::lock_guard<std::mutex> lock(m_gameThreadMutex);
      ProcessPushJob(policy, std::move(job), m_gameThreadJobs);
   }

   void InterThreadCommunicationMgr::ProcessPushLuaThreadJob(const eEnqueueJobPolicy policy, Job &&job)
   {
      std::lock_guard<std::mutex> lock(m_luaThreadMutex);
      ProcessPushJob(policy, std::move(job), m_luaThreadJobs);
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
      auto &renderThreadChain = mRenderThreadSwapChain.GetDequeByIndex(uint8_t(mRenderThreadSwapChain.ReadChainType.load()));
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
      mRenderThreadSwapChain.ReadChainType = mRenderThreadSwapChain.ReadChainType == eReadChainType::READ_1 ? eReadChainType::READ_2 : eReadChainType::READ_1;
      mRenderThreadSwapChain.WriteChainType = mRenderThreadSwapChain.WriteChainType == eWriteChainType::WRITE_1 ? eWriteChainType::WRITE_2 : eWriteChainType::WRITE_1;
   }
}

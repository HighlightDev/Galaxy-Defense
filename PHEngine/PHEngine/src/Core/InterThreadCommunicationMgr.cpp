#include "InterThreadCommunicationMgr.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/CommonCore/Assertion.h"

#include <iostream>
#include <algorithm>

using namespace Game;
using namespace Graphics::Renderer;

namespace Thread
{

   InterThreadCommunicationMgr::InterThreadCommunicationMgr()
       : mRenderThreadSwapChain()
   {
   }

   InterThreadCommunicationMgr::~InterThreadCommunicationMgr()
   {
   }

   void InterThreadCommunicationMgr::SetSceneRendererWP(std::weak_ptr<DeferredShadingSceneRenderer> sceneRenderer)
   {
      mSceneRenderer = sceneRenderer;
   }

   void InterThreadCommunicationMgr::SetSceneWP(std::weak_ptr<Scene> scene)
   {
      mScene = scene;
   }

   std::weak_ptr<DeferredShadingSceneRenderer> InterThreadCommunicationMgr::TryGetSceneRendererWP() const
   {
      return mSceneRenderer;
   }

   std::weak_ptr<Scene> InterThreadCommunicationMgr::TryGetSceneWP() const
   {
      return mScene;
   }

   void InterThreadCommunicationMgr::EmplaceGameThreadJob(const EnqueueJobPolicy policy, Job &&job)
   {
      ProcessPushGameThreadJob(policy, std::move(job));
   }

   void InterThreadCommunicationMgr::EmplaceRenderThreadJob(const EnqueueJobPolicy policy, Job &&job)
   {
      ProcessPushRenderThreadJob(policy, std::move(job));
   }

   void InterThreadCommunicationMgr::ProcessPushRenderThreadJob(const EnqueueJobPolicy policy, Job &&job)
   {
      std::lock_guard<std::mutex> lock(mRenderThreadSwapChain.StoreOperationMutex);
      ProcessPushJob(policy, std::move(job), mRenderThreadSwapChain.GetDequeByIndex(uint8_t(mRenderThreadSwapChain.WriteChainType.load())));
   }

   void InterThreadCommunicationMgr::ProcessPushGameThreadJob(const EnqueueJobPolicy policy, Job &&job)
   {
      std::lock_guard<std::mutex> lock(m_gameThreadMutex);
      ProcessPushJob(policy, std::move(job), m_gameThreadJobs);
   }

   void InterThreadCommunicationMgr::ProcessPushJob(const EnqueueJobPolicy policy, Job &&job, std::deque<Job> &jobs)
   {
      switch (policy)
      {
      case EnqueueJobPolicy::PUSH_ANYWAY:
      {
         jobs.emplace_back(std::move(job));
         break;
      }
      case EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH:
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
      case EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH:
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

   void InterThreadCommunicationMgr::SwapRenderThreadChain()
   {
      std::lock_guard<std::mutex> lock(mRenderThreadSwapChain.StoreOperationMutex);
      mRenderThreadSwapChain.ReadChainType = mRenderThreadSwapChain.ReadChainType == eReadChainType::READ_1 ? eReadChainType::READ_2 : eReadChainType::READ_1;
      mRenderThreadSwapChain.WriteChainType = mRenderThreadSwapChain.WriteChainType == eWriteChainType::WRITE_1 ? eWriteChainType::WRITE_2 : eWriteChainType::WRITE_1;
   }
}

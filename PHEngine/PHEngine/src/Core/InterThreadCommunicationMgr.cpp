#include "InterThreadCommunicationMgr.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

#include <iostream>
#include <algorithm>

using namespace Game;
using namespace Graphics::Renderer;

namespace Thread
{

   InterThreadCommunicationMgr::InterThreadCommunicationMgr()
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

   std::weak_ptr<Scene> InterThreadCommunicationMgr::TryGetSceneWP() const {
      return mScene;
   }

   void InterThreadCommunicationMgr::EmplaceGameThreadJob(const EnqueueJobPolicy policy, Job job)
   {
      std::lock_guard<std::mutex> lock(m_gameThreadMutex);
      ProcessPushGameThreadJob(policy, job);
   }

   void InterThreadCommunicationMgr::EmplaceRenderThreadJob(const EnqueueJobPolicy policy, Job job)
   {
      std::lock_guard<std::mutex> lock(m_renderThreadMutex);
      ProcessPushRenderThreadJob(policy, job);
   }

   void InterThreadCommunicationMgr::ProcessPushRenderThreadJob(const EnqueueJobPolicy policy, Job job)
   {
      ProcessPushJob(policy, job, m_renderThreadJobs);
   }

   void InterThreadCommunicationMgr::ProcessPushGameThreadJob(const EnqueueJobPolicy policy, Job job)
   {
      ProcessPushJob(policy, job, m_gameThreadJobs);
   }

   void InterThreadCommunicationMgr::ProcessPushJob(const EnqueueJobPolicy policy, Job job, std::deque<Job>& jobs)
   {
      switch (policy)
      {
         case EnqueueJobPolicy::PUSH_ANYWAY:
         {
            jobs.emplace_back(job);
            break;
         }
         case EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH:
         {
            const auto duplicateIt = std::find_if(jobs.begin(), jobs.end(),
               [&](const Job& collectionJob)
            {
               return (collectionJob.GetCreatorObjectId() == job.GetCreatorObjectId() && collectionJob.GetFunctionId() == job.GetFunctionId());
            });

            if (jobs.end() == duplicateIt)
            {
               jobs.emplace_back(job);
            }

            break;
         }
         case EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH:
         {
            const auto duplicateIt = std::find_if(jobs.begin(), jobs.end(),
               [&](const Job& collectionJob)
            {
               return (collectionJob.GetCreatorObjectId() == job.GetCreatorObjectId() && collectionJob.GetFunctionId() == job.GetFunctionId());
            });

            if (jobs.end() == duplicateIt)
            {
               jobs.emplace_back(job);
            }
            else
            {
               *(duplicateIt) = job;
            }

            break;
         }
      }
   }

   void InterThreadCommunicationMgr::SpinGameThreadJobs()
   {
      //using Clock_t = std::chrono::high_resolution_clock;

      //typename Clock_t::time_point start_time = Clock_t::now();

      auto countGameThreadJobs = m_gameThreadJobs.size();
      std::lock_guard<std::mutex> lock(m_gameThreadMutex);
      while (AreGameJobsAwaiting())
      {
         auto jobIt = m_gameThreadJobs.begin();
         (*jobIt)();
         m_gameThreadJobs.pop_front();
      }
   }

   void InterThreadCommunicationMgr::SpinRenderThreadJobs()
   {
      //using Clock_t = std::chrono::high_resolution_clock;

      //typename Clock_t::time_point start_time = Clock_t::now();
      {

         auto countRenderThreadJobs = m_renderThreadJobs.size();
         std::lock_guard<std::mutex> lock(m_renderThreadMutex);
         while (countRenderThreadJobs)
         {
            auto jobIt = m_renderThreadJobs.begin();
            (*jobIt)();
            m_renderThreadJobs.pop_front();
            --countRenderThreadJobs;
         }
      }
   }

   bool InterThreadCommunicationMgr::AreGameJobsAwaiting() const {

      return m_gameThreadJobs.size() > 0;
   }

   bool InterThreadCommunicationMgr::AreRenderJobsAwaiting() const {

      return m_renderThreadJobs.size() > 0;
   }
}

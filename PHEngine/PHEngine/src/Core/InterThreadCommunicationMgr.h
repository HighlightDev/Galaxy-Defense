#pragma once

#include <vector>
#include <mutex>

#include "Job.h"

namespace Graphics
{
   namespace Renderer {
      class DeferredShadingSceneRenderer;
   }
}

namespace Game
{
   class Scene;
}

namespace Thread
{

#define ENQUEUE_RENDER_THREAD_JOB(InterThreadMgrInstance, Policy, JOB) \
InterThreadMgrInstance.EmplaceRenderThreadJob(Policy, std::move(JOB))

#define ENQUEUE_GAME_THREAD_JOB(InterThreadMgrInstance, Policy, JOB) \
InterThreadMgrInstance.EmplaceGameThreadJob(Policy, std::move(JOB))

#define SPIN_RENDER_THREAD_JOBS(InterThreadMgrInstance) \
InterThreadMgrInstance.SpinRenderThreadJobs();

#define SPIN_GAME_THREAD_JOBS(InterThreadMgrInstance) \
InterThreadMgrInstance.SpinGameThreadJobs();

   enum class EnqueueJobPolicy
   {
      IF_DUPLICATE_NO_PUSH,
      IF_DUPLICATE_REPLACE_AND_PUSH,
      PUSH_ANYWAY
   };

   class InterThreadCommunicationMgr
   {
      std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> mSceneRenderer;
      std::weak_ptr<Game::Scene> mScene;

      const size_t JobPoolCapacity = 80;

      std::mutex m_gameThreadMutex;

      std::mutex m_renderThreadMutex;

      std::vector<Job> m_gameThreadJobs;

      std::vector<Job> m_renderThreadJobs;

   public:

      InterThreadCommunicationMgr();

      ~InterThreadCommunicationMgr();

      void EmplaceGameThreadJob(const EnqueueJobPolicy, Job&& job);

      void EmplaceRenderThreadJob(const EnqueueJobPolicy, Job&& job);

      /* @ Should be executed only on game thread! */
      void SpinGameThreadJobs();

      /* @ Should be executed only on render thread! */
      void SpinRenderThreadJobs();

      void SetSceneRendererWP(std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> sceneRenderer);
      
      void SetSceneWP(std::weak_ptr<Game::Scene> scene);

      std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> TryGetSceneRendererWP() const;

      std::weak_ptr<Game::Scene> TryGetSceneWP() const;

   private:

      void ProcessPushRenderThreadJob(const EnqueueJobPolicy policy, const Job& job);
      void ProcessPushGameThreadJob(const EnqueueJobPolicy policy, const Job& job);
      void ProcessPushJob(const EnqueueJobPolicy policy, const Job& job, std::vector<Job>& jobs);

      inline bool AreGameJobsAwaiting() const {

         return m_gameThreadJobs.size() > 0;
      }

      inline bool AreRenderJobsAwaiting() const {

         return m_renderThreadJobs.size() > 0;
      }
   };

}


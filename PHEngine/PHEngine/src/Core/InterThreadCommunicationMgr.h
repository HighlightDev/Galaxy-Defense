#pragma once

#include <vector>
#include <deque>
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

      std::deque<Job> m_gameThreadJobs;

      std::deque<Job> m_renderThreadJobs;

   public:

      InterThreadCommunicationMgr();

      ~InterThreadCommunicationMgr();

      void EmplaceGameThreadJob(const EnqueueJobPolicy, Job job);

      void EmplaceRenderThreadJob(const EnqueueJobPolicy, Job job);

      /* @ Should be executed only on game thread! */
      void SpinGameThreadJobs();

      /* @ Should be executed only on render thread! */
      void SpinRenderThreadJobs();

      void SetSceneRendererWP(std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> sceneRenderer);
      
      void SetSceneWP(std::weak_ptr<Game::Scene> scene);

      std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> TryGetSceneRendererWP() const;

      std::weak_ptr<Game::Scene> TryGetSceneWP() const;

   private:

      void ProcessPushRenderThreadJob(const EnqueueJobPolicy policy, Job job);
      void ProcessPushGameThreadJob(const EnqueueJobPolicy policy, Job job);
      void ProcessPushJob(const EnqueueJobPolicy policy, Job job, std::deque<Job>& jobs);

      bool AreGameJobsAwaiting() const;

      bool AreRenderJobsAwaiting() const;
   };

}


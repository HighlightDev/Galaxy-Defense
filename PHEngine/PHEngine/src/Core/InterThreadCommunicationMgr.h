#pragma once

#include <vector>
#include <deque>
#include <mutex>
#include <array>

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
   private:

      enum class eReadChainType : uint8_t {
         READ_1 = 0,
         READ_2 = 1
      };

      enum class eWriteChainType : uint8_t {
         WRITE_1 = 0,
         WRITE_2 = 1
      };

      struct TasksSwapChain
      {
         eReadChainType ReadChainType = eReadChainType::READ_1;
         eWriteChainType WriteChainType = eWriteChainType::WRITE_2;

         std::mutex StoreOpMutex;

         std::array<std::deque<Job>, 2> Tasks;
      };

   private:
      
      std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> mSceneRenderer;

      std::weak_ptr<Game::Scene> mScene;

      std::mutex m_gameThreadMutex;

      std::deque<Job> m_gameThreadJobs;

      TasksSwapChain mRenderThreadSwapChain;

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

      void SwapRenderThreadChain();
   };

}


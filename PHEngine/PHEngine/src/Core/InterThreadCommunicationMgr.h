#pragma once

#include <vector>
#include <deque>
#include <mutex>
#include <array>
#include <memory>
#include <thread>
#include <atomic>

#include "Job.h"

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

namespace Graphics
{
   namespace Renderer
   {
      class DeferredShadingSceneRenderer;
   }
}

namespace EngineCore
{
   class Scene;
}

namespace Thread
{
   enum class eEnqueueJobPolicy
   {
      IF_DUPLICATE_NO_PUSH,
      IF_DUPLICATE_REPLACE_AND_PUSH,
      PUSH_ANYWAY
   };

   enum class eReadChainType : uint8_t
   {
      READ_1 = 0,
      READ_2 = 1
   };

   enum class eWriteChainType : uint8_t
   {
      WRITE_1 = 0,
      WRITE_2 = 1
   };

   struct TasksSwapChain
   {
      std::mutex StoreOperationMutex;
      std::atomic<eReadChainType> ReadChainType = {eReadChainType::READ_1};
      std::atomic<eWriteChainType> WriteChainType = {eWriteChainType::WRITE_2};

      // make sure we won't get false sharing for our jobs
      alignas(hardware_destructive_interference_size) std::deque<Job> Jobs1;
      alignas(hardware_destructive_interference_size) std::deque<Job> Jobs2;

      inline std::deque<Job> &GetDequeByIndex(uint8_t index)
      {
         if (0 == index)
            return Jobs1;
         else
            return Jobs2;
      }
   };

   class InterThreadCommunicationMgr
   {
      std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> mSceneRenderer;

      std::weak_ptr<EngineCore::Scene> mScene;

      std::mutex m_gameThreadMutex;

      std::deque<Job> m_gameThreadJobs;

      TasksSwapChain mRenderThreadSwapChain;

   public:
      InterThreadCommunicationMgr();

      ~InterThreadCommunicationMgr();

      void EmplaceGameThreadJob(const eEnqueueJobPolicy, Job &&job);

      void EmplaceRenderThreadJob(const eEnqueueJobPolicy, Job &&job);

      /* @ Should be executed only on game thread! */
      void SpinGameThreadJobs();

      /* @ Should be executed only on render thread! */
      void SpinRenderThreadJobs();

      void SetSceneRendererWP(std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> sceneRenderer);

      void SetSceneWP(std::weak_ptr<EngineCore::Scene> scene);

      std::weak_ptr<Graphics::Renderer::DeferredShadingSceneRenderer> TryGetSceneRendererWP() const;

      std::weak_ptr<EngineCore::Scene> TryGetSceneWP() const;

   private:
      void ProcessPushRenderThreadJob(const eEnqueueJobPolicy policy, Job &&job);

      void ProcessPushGameThreadJob(const eEnqueueJobPolicy policy, Job &&job);

      void ProcessPushJob(const eEnqueueJobPolicy policy, Job &&job, std::deque<Job> &jobs);

      void SwapRenderThreadChain();
   };

}

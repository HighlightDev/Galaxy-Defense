#pragma once

#include "Job.h"

#include <array>
#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

namespace Graphics {
namespace Renderer {
class SceneRenderer;
}
} // namespace Graphics

namespace EngineCore {
class Scene;

namespace Scripts {
class LuaScriptProcessor;
}
} // namespace EngineCore

namespace Thread {
enum class eEnqueueJobPolicy { IF_DUPLICATE_NO_PUSH, IF_DUPLICATE_REPLACE, PUSH_ANYWAY };

enum eReadChainType : uint8_t { READ_1 = 0, READ_2 = 1 };

enum eWriteChainType : uint8_t { WRITE_1 = 0, WRITE_2 = 1 };

struct TasksSwapChain {
    std::mutex StoreOperationMutex;
    uint8_t ReadChainType = {eReadChainType::READ_1};
    uint8_t WriteChainType = {eWriteChainType::WRITE_2};

    // make sure we won't get false sharing for our jobs
    alignas(hardware_destructive_interference_size) std::deque<Job> Jobs1;
    alignas(hardware_destructive_interference_size) std::deque<Job> Jobs2;

    inline std::deque<Job>& GetDequeByIndex(const uint8_t index)
    {
        return index == 0 ? Jobs1 : Jobs2;
    }
};

class InterThreadCommunicationMgr {
    std::weak_ptr<Graphics::Renderer::SceneRenderer> mSceneRenderer;

    std::weak_ptr<EngineCore::Scene> mScene;

    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> mLuaScriptProcessor;

    std::mutex m_gameThreadMutex;

    std::deque<Job> m_gameThreadJobs;

    std::deque<Job> m_luaThreadJobs;

    std::mutex m_luaThreadMutex;

    TasksSwapChain mRenderThreadSwapChain;

    std::atomic_bool mIsAllowedPushGameThreadJobs{true};

    std::atomic_bool mIsAllowedPushLuaThreadJobs{true};

public:
    InterThreadCommunicationMgr();

    ~InterThreadCommunicationMgr();

    void ExecuteOnRenderThread(
        const eEnqueueJobPolicy policy,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        std::function<void(void)> gameThreadJobCallback);

    void ExecuteOnGameThread(
        const eEnqueueJobPolicy policy,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        std::function<void(void)> renderThreadJobCallback);

    void ExecuteOnLuaThread(
        const eEnqueueJobPolicy policy,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        std::function<void(void)> luaThreadJobCallback);

    /* @ Should be executed only on game thread! */
    void SpinGameThreadJobs();

    /* @ Should be executed only on render thread! */
    void SpinRenderThreadJobs();

    void SpinLuaThreadJob();

    void ClearGameThreadJobs();

    void ClearLuaThreadJobs();

    void SetIsAllowedPushGameThreadJobs(const bool isAllowed);

    void SetIsAllowedPushLuaThreadJobs(const bool isAllowed);

    void SetSceneRendererWP(std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRenderer);

    void SetSceneWP(std::weak_ptr<EngineCore::Scene> scene);

    void SetLuaScriptProcessorWP(const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>& scriptProcessor);

    std::weak_ptr<Graphics::Renderer::SceneRenderer> GetSceneRendererWP() const;

    std::weak_ptr<EngineCore::Scene> GetSceneWP() const;

    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> GetLuaScriptProcessor() const;

private:
    void ProcessPushRenderThreadJob(const eEnqueueJobPolicy policy, Job&& job);

    void ProcessPushGameThreadJob(const eEnqueueJobPolicy policy, Job&& job);

    void ProcessPushLuaThreadJob(const eEnqueueJobPolicy policy, Job&& job);

    void ProcessPushJob(const eEnqueueJobPolicy policy, Job&& job, std::deque<Job>& jobs);

    void SwapRenderThreadChain();
};

} // namespace Thread

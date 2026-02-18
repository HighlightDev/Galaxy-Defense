#pragma once

#include "Job.h"

#include <atomic>
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

/**
 * @struct TasksSwapChain
 * @brief Manages two job queues (swap chains) for inter-thread communication, ensuring thread safety and minimizing false
 * sharing.
 *
 * This structure provides two separate job vectors (`Jobs1` and `Jobs2`), each aligned to avoid hardware destructive
 * interference, which helps prevent performance degradation due to false sharing in multi-threaded environments. The
 * `StoreOperationMutex` protects operations on the swap chain. The `ReadChainType` and `WriteChainType` indicate which chain is
 * currently used for reading and writing.
 *
 * @var std::mutex StoreOperationMutex
 *      Mutex to synchronize access to the swap chain operations.
 * @var uint8_t ReadChainType
 *      Indicates the current read chain type.
 * @var uint8_t WriteChainType
 *      Indicates the current write chain type.
 * @var std::vector<TaskJob_t> Jobs1
 *      First job queue, aligned to avoid false sharing.
 * @var std::vector<TaskJob_t> Jobs2
 *      Second job queue, aligned to avoid false sharing.
 *
 * @fn std::vector<TaskJob_t>& GetTasksByIndex(const uint8_t index)
 * @brief Returns a reference to the job vector specified by the index (0 for Jobs1, otherwise Jobs2).
 * @param index Index of the job vector to retrieve.
 * @return Reference to the selected job vector.
 */

using TaskJob_t = Job<
    std::weak_ptr<Graphics::Renderer::SceneRenderer>,
    std::weak_ptr<EngineCore::Scene>,
    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>>;
struct TasksSwapChain {
    std::mutex StoreOperationMutex;
    uint8_t ReadChainType = {eReadChainType::READ_1};
    uint8_t WriteChainType = {eWriteChainType::WRITE_2};

    // make sure we won't get false sharing for our jobs
    alignas(hardware_destructive_interference_size) std::vector<TaskJob_t> Jobs1;
    alignas(hardware_destructive_interference_size) std::vector<TaskJob_t> Jobs2;

    inline std::vector<TaskJob_t>& GetTasksByIndex(const uint8_t index)
    {
        return index == 0 ? Jobs1 : Jobs2;
    }
};

/**
 * @class InterThreadCommunicationMgr
 * @brief Manages communication and job scheduling between game, render, and Lua threads.
 *
 * This class provides mechanisms to enqueue and execute jobs on different threads (game, render, Lua)
 * in a thread-safe manner. It maintains job queues for each thread and controls access using mutexes
 * and atomic flags. The class also manages weak references to core engine components such as the scene,
 * scene renderer, and Lua script processor.
 *
 * Main Responsibilities:
 * - Enqueue jobs to game, render, and Lua threads with specific policies.
 * - Execute jobs on respective threads via spin methods.
 * - Control whether jobs can be pushed to game or Lua thread queues.
 * - Manage weak pointers to engine components for safe cross-thread access.
 * - Provide thread-safe job queue clearing and swapping mechanisms.
 *
 * Usage Notes:
 * - SpinGameThreadJobs() and SpinRenderThreadJobs() should be called only on their respective threads.
 * - Thread safety is ensured via mutexes and atomic flags.
 * - Jobs are represented by the Job type and are processed according to the specified enqueue policy.
 */
class InterThreadCommunicationMgr {
    std::weak_ptr<Graphics::Renderer::SceneRenderer> mSceneRenderer;

    std::weak_ptr<EngineCore::Scene> mScene;

    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> mLuaScriptProcessor;

    std::mutex m_gameThreadMutex;
    std::vector<TaskJob_t> m_gameThreadPendingJobs;
    std::vector<TaskJob_t> m_gameThreadExecutingJobs;
    std::unordered_map<uint64_t, std::vector<int32_t>> m_gameThreadJobsHashes;

    std::mutex m_luaThreadMutex;
    std::vector<TaskJob_t> m_luaThreadPendingJobs;
    std::vector<TaskJob_t> m_luaThreadExecutingJobs;
    std::unordered_map<uint64_t, std::vector<int32_t>> m_luaThreadJobsHashes;

    TasksSwapChain mRenderThreadSwapChain;
    std::unordered_map<uint64_t, std::vector<int32_t>> m_renderThreadJobsHashes;

    std::atomic_bool mIsAllowedPushGameThreadJobs{true};

    std::atomic_bool mIsAllowedPushLuaThreadJobs{true};

public:
    InterThreadCommunicationMgr();

    ~InterThreadCommunicationMgr();

    void ExecuteOnRenderThread(
        const eEnqueueJobPolicy policy,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        std::function<void(
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp)> gameThreadJobCallback);

    void ExecuteOnGameThread(
        const eEnqueueJobPolicy policy,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        std::function<void(
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp)> renderThreadJobCallback);

    void ExecuteOnLuaThread(
        const eEnqueueJobPolicy policy,
        const int32_t creatorObjectId,
        const uint64_t functionId,
        std::function<void(
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp)> luaThreadJobCallback);

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
    void ProcessPushRenderThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job);

    void ProcessPushGameThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job);

    void ProcessPushLuaThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job);

    void ProcessPushJob(
        const eEnqueueJobPolicy policy,
        TaskJob_t&& job,
        std::vector<TaskJob_t>& jobs,
        std::unordered_map<uint64_t, std::vector<int32_t>>& jobsHashes);

    void SwapRenderThreadChain();
};

} // namespace Thread

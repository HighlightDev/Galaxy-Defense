#pragma once

#include "Core/CommonCore/EngineConstants.h"
#include "Job.h"

#include <array>
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

using TaskJob_t = Job<
    std::weak_ptr<Graphics::Renderer::SceneRenderer>,
    std::weak_ptr<EngineCore::Scene>,
    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor>>;

using JobsBuffer_t = std::array<TaskJob_t, EngineConstants::c_renderThreadJobsPoolsSize>;

/**
 * @struct TasksSwapChain
 * @brief Double-buffered, fixed-size job swap chain for inter-thread communication, thread-safe and false-sharing aware.
 *
 * Holds two fixed-size job buffers (`Jobs1`/`Jobs2`) used as a read/write double buffer: producers append to the write
 * buffer (under `StoreOperationMutex`) while the owning thread drains the read buffer, then the chains are swapped. Each
 * buffer tracks how many slots are filled via its own step index (`mBufferStepIndex1`/`2`). Buffers and step indices are
 * cache-line aligned to avoid false sharing. `ReadChainType`/`WriteChainType` select the current read/write buffer.
 */
struct TasksSwapChain {
    std::mutex StoreOperationMutex;
    uint8_t ReadChainType = {eReadChainType::READ_1};
    uint8_t WriteChainType = {eWriteChainType::WRITE_2};

    // make sure we won't get false sharing for our cycle-buffer of jobs
    alignas(hardware_destructive_interference_size) JobsBuffer_t Jobs1;
    alignas(hardware_destructive_interference_size) JobsBuffer_t Jobs2;
    alignas(hardware_destructive_interference_size) uint16_t mBufferStepIndex1{0};
    alignas(hardware_destructive_interference_size) uint16_t mBufferStepIndex2{0};

    inline JobsBuffer_t& GetTasksByIndex(const uint8_t index)
    {
        return index == 0 ? Jobs1 : Jobs2;
    }

    inline uint16_t& GetBufferStepIndexByChainType(const uint8_t chainType)
    {
        return chainType == 0 ? mBufferStepIndex1 : mBufferStepIndex2;
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

    TasksSwapChain mGameThreadSwapChain;
    std::unordered_map<uint64_t, std::vector<int32_t>> m_gameThreadJobsHashes;

    TasksSwapChain mLuaThreadSwapChain;
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
        uint16_t& writeBufferIndex,
        JobsBuffer_t& jobs,
        std::unordered_map<uint64_t, std::vector<int32_t>>& jobsHashes);

    // Drains the swap chain's read buffer (invoking each job), resets it, clears the dedup hashes and flips the chains.
    // Must be called on the thread that owns the swap chain (its read buffer is drained without the store mutex held).
    void SpinThreadJobs(TasksSwapChain& swapChain, std::unordered_map<uint64_t, std::vector<int32_t>>& jobsHashes);

    void SwapChain(TasksSwapChain& swapChain);
};

} // namespace Thread

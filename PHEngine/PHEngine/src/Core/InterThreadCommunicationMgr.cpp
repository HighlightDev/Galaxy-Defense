#include "InterThreadCommunicationMgr.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

#include <TinyLogger/LogInterface.h>

#include <algorithm>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Renderer;
using namespace TinyLogger;

#define flipBits(n, b) ((n) ^ ((1u << (b)) - 1))

namespace Thread {
InterThreadCommunicationMgr::InterThreadCommunicationMgr()
    : mRenderThreadSwapChain()
{
    LogInfo("InterThreadCommunicationMgr::ctor");
}

InterThreadCommunicationMgr::~InterThreadCommunicationMgr()
{
    LogInfo("InterThreadCommunicationMgr::dctor");
}

void InterThreadCommunicationMgr::SetSceneRendererWP(std::weak_ptr<SceneRenderer> sceneRenderer)
{
    mSceneRenderer = sceneRenderer;
}

void InterThreadCommunicationMgr::SetSceneWP(std::weak_ptr<Scene> scene)
{
    mScene = scene;
}

void InterThreadCommunicationMgr::SetLuaScriptProcessorWP(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

std::weak_ptr<SceneRenderer> InterThreadCommunicationMgr::GetSceneRendererWP() const
{
    return mSceneRenderer;
}

std::weak_ptr<Scene> InterThreadCommunicationMgr::GetSceneWP() const
{
    return mScene;
}

std::weak_ptr<LuaScriptProcessor> InterThreadCommunicationMgr::GetLuaScriptProcessor() const
{
    return mLuaScriptProcessor;
}

void InterThreadCommunicationMgr::ExecuteOnRenderThread(
    eEnqueueJobPolicy policy,
    const int32_t creatorObjectId,
    const uint64_t functionId,
    std::function<void(
        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
        std::weak_ptr<EngineCore::Scene> sceneWp,
        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp)> renderThreadJobCallback)
{
    ProcessPushRenderThreadJob(policy, TaskJob_t(creatorObjectId, functionId, renderThreadJobCallback));
}

void InterThreadCommunicationMgr::ExecuteOnGameThread(
    eEnqueueJobPolicy policy,
    const int32_t creatorObjectId,
    const uint64_t functionId,
    std::function<void(
        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
        std::weak_ptr<EngineCore::Scene> sceneWp,
        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp)> gameThreadJobCallback)
{
    ProcessPushGameThreadJob(policy, TaskJob_t(creatorObjectId, functionId, gameThreadJobCallback));
}

void InterThreadCommunicationMgr::ExecuteOnLuaThread(
    eEnqueueJobPolicy policy,
    const int32_t creatorObjectId,
    const uint64_t functionId,
    std::function<void(
        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
        std::weak_ptr<EngineCore::Scene> sceneWp,
        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp)> luaThreadJobCallback)
{
    ProcessPushLuaThreadJob(policy, TaskJob_t(creatorObjectId, functionId, luaThreadJobCallback));
}

void InterThreadCommunicationMgr::ProcessPushRenderThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job)
{
    std::lock_guard<std::mutex> lock(mRenderThreadSwapChain.StoreOperationMutex);
    ProcessPushJob(
        policy,
        std::move(job),
        mRenderThreadSwapChain.GetBufferStepIndexByChainType(mRenderThreadSwapChain.WriteChainType),
        mRenderThreadSwapChain.GetTasksByIndex(mRenderThreadSwapChain.WriteChainType),
        m_renderThreadJobsHashes);
}

void InterThreadCommunicationMgr::ProcessPushGameThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job)
{
    if (mIsAllowedPushGameThreadJobs.load(std::memory_order::acquire)) {
        std::lock_guard<std::mutex> lock(mGameThreadSwapChain.StoreOperationMutex);
        ProcessPushJob(
            policy,
            std::move(job),
            mGameThreadSwapChain.GetBufferStepIndexByChainType(mGameThreadSwapChain.WriteChainType),
            mGameThreadSwapChain.GetTasksByIndex(mGameThreadSwapChain.WriteChainType),
            m_gameThreadJobsHashes);
    }
}

void InterThreadCommunicationMgr::ProcessPushLuaThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job)
{
    if (mIsAllowedPushLuaThreadJobs.load(std::memory_order::acquire)) {
        std::lock_guard<std::mutex> lock(mLuaThreadSwapChain.StoreOperationMutex);
        ProcessPushJob(
            policy,
            std::move(job),
            mLuaThreadSwapChain.GetBufferStepIndexByChainType(mLuaThreadSwapChain.WriteChainType),
            mLuaThreadSwapChain.GetTasksByIndex(mLuaThreadSwapChain.WriteChainType),
            m_luaThreadJobsHashes);
    }
}

void InterThreadCommunicationMgr::ProcessPushJob(
    const eEnqueueJobPolicy policy,
    TaskJob_t&& job,
    uint16_t& writeBufferIndex,
    JobsBuffer_t& jobs,
    std::unordered_map<uint64_t, std::vector<int32_t>>& jobsHashes)
{
    const uint64_t jobHash = job.GetHash();

    // Appends to the next free slot: records the slot in the dedup map (before the bump) then advances the step index.
    const auto appendJob = [&](TaskJob_t&& jobToPush) {
        ext_assert(
            writeBufferIndex < jobs.size(),
            "InterThreadCommunicationMgr::ProcessPushJob: job buffer overflow, increase c_renderThreadJobsPoolsSize");
        jobsHashes[jobHash].push_back(writeBufferIndex);
        jobs[writeBufferIndex++] = std::move(jobToPush);
    };

    if (eEnqueueJobPolicy::PUSH_ANYWAY == policy) {
        appendJob(std::move(job));
    } else if (eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH == policy) {
        if (jobsHashes.find(jobHash) == jobsHashes.end()) {
            appendJob(std::move(job));
        }
    } else if (eEnqueueJobPolicy::IF_DUPLICATE_REPLACE == policy) {
        if (jobsHashes.find(jobHash) == jobsHashes.end()) {
            appendJob(std::move(job));
        } else {
            const auto& jobIndexes = jobsHashes[jobHash];
            for (const auto& jobIndex : jobIndexes) {
                jobs[jobIndex] = job;
            }
        }
    }
}

void InterThreadCommunicationMgr::SpinThreadJobs(
    TasksSwapChain& swapChain, std::unordered_map<uint64_t, std::vector<int32_t>>& jobsHashes)
{
    {
        std::lock_guard<std::mutex> lock(swapChain.StoreOperationMutex);
        SwapChain(swapChain);
        jobsHashes.clear();
    }

    auto& readJobs = swapChain.GetTasksByIndex(swapChain.ReadChainType);
    auto& readJobsStepIndex = swapChain.GetBufferStepIndexByChainType(swapChain.ReadChainType);

    for (uint16_t i = 0; i < readJobsStepIndex; ++i) {
        std::invoke(readJobs[i], mSceneRenderer, mScene, mLuaScriptProcessor);
    }
    readJobsStepIndex = 0;
}

void InterThreadCommunicationMgr::SpinGameThreadJobs()
{
    SpinThreadJobs(mGameThreadSwapChain, m_gameThreadJobsHashes);
}

void InterThreadCommunicationMgr::SpinRenderThreadJobs()
{
    SpinThreadJobs(mRenderThreadSwapChain, m_renderThreadJobsHashes);
}

void InterThreadCommunicationMgr::SpinLuaThreadJob()
{
    SpinThreadJobs(mLuaThreadSwapChain, m_luaThreadJobsHashes);
}

void InterThreadCommunicationMgr::SwapChain(TasksSwapChain& swapChain)
{
    swapChain.ReadChainType = flipBits(swapChain.ReadChainType, 1);
    swapChain.WriteChainType = flipBits(swapChain.WriteChainType, 1);
}

void InterThreadCommunicationMgr::ClearGameThreadJobs()
{
    std::lock_guard<std::mutex> lock(mGameThreadSwapChain.StoreOperationMutex);
    // Reset both buffers to empty and release any captured resources held by pending jobs (called on level unload).
    mGameThreadSwapChain.Jobs1.fill(TaskJob_t{});
    mGameThreadSwapChain.Jobs2.fill(TaskJob_t{});
    mGameThreadSwapChain.mBufferStepIndex1 = 0;
    mGameThreadSwapChain.mBufferStepIndex2 = 0;
    m_gameThreadJobsHashes.clear();
}

void InterThreadCommunicationMgr::ClearLuaThreadJobs()
{
    std::lock_guard<std::mutex> lock(mLuaThreadSwapChain.StoreOperationMutex);
    mLuaThreadSwapChain.Jobs1.fill(TaskJob_t{});
    mLuaThreadSwapChain.Jobs2.fill(TaskJob_t{});
    mLuaThreadSwapChain.mBufferStepIndex1 = 0;
    mLuaThreadSwapChain.mBufferStepIndex2 = 0;
    m_luaThreadJobsHashes.clear();
}

void InterThreadCommunicationMgr::SetIsAllowedPushGameThreadJobs(const bool isAllowed)
{
    mIsAllowedPushGameThreadJobs.store(isAllowed, std::memory_order::release);
}

void InterThreadCommunicationMgr::SetIsAllowedPushLuaThreadJobs(const bool isAllowed)
{
    mIsAllowedPushLuaThreadJobs.store(isAllowed, std::memory_order::release);
}
} // namespace Thread
#undef flipBits

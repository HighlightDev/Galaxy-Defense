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
        mRenderThreadSwapChain.GetTasksByIndex(mRenderThreadSwapChain.WriteChainType),
        m_renderThreadJobsHashes);
}

void InterThreadCommunicationMgr::ProcessPushGameThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job)
{
    if (mIsAllowedPushGameThreadJobs.load(std::memory_order::acquire)) {
        std::lock_guard<std::mutex> lock(m_gameThreadMutex);
        ProcessPushJob(policy, std::move(job), m_gameThreadPendingJobs, m_gameThreadJobsHashes);
    }
}

void InterThreadCommunicationMgr::ProcessPushLuaThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job)
{
    if (mIsAllowedPushLuaThreadJobs.load(std::memory_order::acquire)) {
        std::lock_guard<std::mutex> lock(m_luaThreadMutex);
        ProcessPushJob(policy, std::move(job), m_luaThreadPendingJobs, m_luaThreadJobsHashes);
    }
}

void InterThreadCommunicationMgr::ProcessPushJob(
    const eEnqueueJobPolicy policy,
    TaskJob_t&& job,
    std::vector<TaskJob_t>& jobs,
    std::unordered_map<uint64_t, std::vector<int32_t>>& jobsHashes)
{
    const uint64_t jobHash = job.GetHash();
    if (eEnqueueJobPolicy::PUSH_ANYWAY == policy) {
        const auto newJobIndex = jobs.size();
        jobs.emplace_back(std::move(job));
        jobsHashes[jobHash].push_back(newJobIndex);
    } else if (eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH == policy) {
        if (jobsHashes.find(jobHash) == jobsHashes.end()) {
            const auto newJobIndex = jobs.size();
            jobs.emplace_back(std::move(job));
            jobsHashes[jobHash].push_back(newJobIndex);
        }
    } else if (eEnqueueJobPolicy::IF_DUPLICATE_REPLACE == policy) {
        if (jobsHashes.find(jobHash) == jobsHashes.end()) {
            const auto newJobIndex = jobs.size();
            jobs.emplace_back(std::move(job));
            jobsHashes[jobHash].push_back(newJobIndex);
        } else {
            const auto& jobIndexes = jobsHashes[jobHash];
            for (const auto& jobIndex : jobIndexes) {
                jobs[jobIndex] = job;
            }
        }
    }
}

void InterThreadCommunicationMgr::SpinGameThreadJobs()
{
    {
        std::lock_guard<std::mutex> lock(m_gameThreadMutex);
        m_gameThreadPendingJobs.swap(m_gameThreadExecutingJobs);
        m_gameThreadJobsHashes.clear();
    }

    for (const auto& job : m_gameThreadExecutingJobs) {
        job(mSceneRenderer, mScene, mLuaScriptProcessor);
    }
    m_gameThreadExecutingJobs.clear();
}

void InterThreadCommunicationMgr::SpinRenderThreadJobs()
{
    auto& renderThreadChain = mRenderThreadSwapChain.GetTasksByIndex(mRenderThreadSwapChain.ReadChainType);

    for (const auto& job : renderThreadChain) {
        job(mSceneRenderer, mScene, mLuaScriptProcessor);
    }
    renderThreadChain.clear();

    std::lock_guard<std::mutex> lock(mRenderThreadSwapChain.StoreOperationMutex);
    m_renderThreadJobsHashes.clear();
    SwapRenderThreadChain();
}

void InterThreadCommunicationMgr::SpinLuaThreadJob()
{
    {
        std::lock_guard<std::mutex> lock(m_luaThreadMutex);
        m_luaThreadPendingJobs.swap(m_luaThreadExecutingJobs);
        m_luaThreadJobsHashes.clear();
    }

    for (const auto& job : m_luaThreadExecutingJobs) {
        job(mSceneRenderer, mScene, mLuaScriptProcessor);
    }
    m_luaThreadExecutingJobs.clear();
}

void InterThreadCommunicationMgr::SwapRenderThreadChain()
{
    mRenderThreadSwapChain.ReadChainType = flipBits(mRenderThreadSwapChain.ReadChainType, 1);
    mRenderThreadSwapChain.WriteChainType = flipBits(mRenderThreadSwapChain.WriteChainType, 1);
}

void InterThreadCommunicationMgr::ClearGameThreadJobs()
{
    std::lock_guard<std::mutex> lock(m_gameThreadMutex);
    m_gameThreadPendingJobs.clear();
    m_gameThreadExecutingJobs.clear();
}

void InterThreadCommunicationMgr::ClearLuaThreadJobs()
{
    std::lock_guard<std::mutex> lock(m_luaThreadMutex);
    m_luaThreadPendingJobs.clear();
    m_luaThreadExecutingJobs.clear();
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

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
    ProcessPushJob(policy, std::move(job), mRenderThreadSwapChain.GetDequeByIndex(mRenderThreadSwapChain.WriteChainType));
}

void InterThreadCommunicationMgr::ProcessPushGameThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job)
{
    if (mIsAllowedPushGameThreadJobs.load(std::memory_order::seq_cst)) {
        std::lock_guard<std::mutex> lock(m_gameThreadMutex);
        ProcessPushJob(policy, std::move(job), m_gameThreadJobs);
    }
}

void InterThreadCommunicationMgr::ProcessPushLuaThreadJob(const eEnqueueJobPolicy policy, TaskJob_t&& job)
{
    if (mIsAllowedPushLuaThreadJobs.load(std::memory_order::seq_cst)) {
        std::lock_guard<std::mutex> lock(m_luaThreadMutex);
        ProcessPushJob(policy, std::move(job), m_luaThreadJobs);
    }
}

void InterThreadCommunicationMgr::ProcessPushJob(const eEnqueueJobPolicy policy, TaskJob_t&& job, std::deque<TaskJob_t>& jobs)
{
    switch (policy) {
    case eEnqueueJobPolicy::PUSH_ANYWAY: {
        jobs.emplace_back(std::move(job));
        break;
    }
    case eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH: {
        const auto duplicateIt = std::find_if(jobs.begin(), jobs.end(), [&](const TaskJob_t& collectionJob) {
            return (
                collectionJob.GetCreatorObjectId() == job.GetCreatorObjectId()
                && collectionJob.GetFunctionId() == job.GetFunctionId());
        });

        if (jobs.end() == duplicateIt) {
            jobs.emplace_back(std::move(job));
        }

        break;
    }
    case eEnqueueJobPolicy::IF_DUPLICATE_REPLACE: {
        const auto duplicateIt = std::find_if(jobs.begin(), jobs.end(), [&](const TaskJob_t& collectionJob) {
            return (
                (collectionJob.GetCreatorObjectId() == job.GetCreatorObjectId())
                && (collectionJob.GetFunctionId() == job.GetFunctionId()));
        });

        if (jobs.end() == duplicateIt) {
            jobs.emplace_back(std::move(job));
        } else {
            *(duplicateIt) = std::move(job);
        }

        break;
    }
    }
}

void InterThreadCommunicationMgr::SpinGameThreadJobs()
{
    std::lock_guard<std::mutex> lock(m_gameThreadMutex);
    auto countGameThreadJobs = m_gameThreadJobs.size();
    while (countGameThreadJobs) {
        auto jobIt = m_gameThreadJobs.begin();
        (*jobIt)(mSceneRenderer, mScene, mLuaScriptProcessor);
        m_gameThreadJobs.pop_front();
        --countGameThreadJobs;
    }
}

void InterThreadCommunicationMgr::SpinRenderThreadJobs()
{
    auto& renderThreadChain = mRenderThreadSwapChain.GetDequeByIndex(mRenderThreadSwapChain.ReadChainType);
    auto countRenderThreadJobs = renderThreadChain.size();

    while (countRenderThreadJobs) {
        auto jobIt = renderThreadChain.begin();
        (*jobIt)(mSceneRenderer, mScene, mLuaScriptProcessor);
        renderThreadChain.pop_front();
        --countRenderThreadJobs;
    }

    SwapRenderThreadChain();
}

void InterThreadCommunicationMgr::SpinLuaThreadJob()
{
    std::lock_guard<std::mutex> lock(m_luaThreadMutex);
    auto countLuaThreadJobs = m_luaThreadJobs.size();
    while (countLuaThreadJobs) {
        auto jobIt = m_luaThreadJobs.begin();
        (*jobIt)(mSceneRenderer, mScene, mLuaScriptProcessor);
        m_luaThreadJobs.pop_front();
        --countLuaThreadJobs;
    }
}

void InterThreadCommunicationMgr::SwapRenderThreadChain()
{
    std::lock_guard<std::mutex> lock(mRenderThreadSwapChain.StoreOperationMutex);
    mRenderThreadSwapChain.ReadChainType = flipBits(mRenderThreadSwapChain.ReadChainType, 1);
    mRenderThreadSwapChain.WriteChainType = flipBits(mRenderThreadSwapChain.WriteChainType, 1);
}

void InterThreadCommunicationMgr::ClearGameThreadJobs()
{
    std::lock_guard<std::mutex> lock(m_gameThreadMutex);
    m_gameThreadJobs.clear();
}

void InterThreadCommunicationMgr::ClearLuaThreadJobs()
{
    std::lock_guard<std::mutex> lock(m_luaThreadMutex);
    m_luaThreadJobs.clear();
}

void InterThreadCommunicationMgr::SetIsAllowedPushGameThreadJobs(const bool isAllowed)
{
    mIsAllowedPushGameThreadJobs.store(isAllowed, std::memory_order::seq_cst);
}

void InterThreadCommunicationMgr::SetIsAllowedPushLuaThreadJobs(const bool isAllowed)
{
    mIsAllowedPushLuaThreadJobs.store(isAllowed, std::memory_order::seq_cst);
}
} // namespace Thread
#undef flipBits

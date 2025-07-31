#include "OverlayManager.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/OverlayManagerLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <algorithm>

using namespace EngineCore;
using namespace EngineCore::Scripts;

namespace EngineCore {
namespace GUI {
OverlayManager::OverlayManager(const std::weak_ptr<Scene>& scene)
    : EngineToLuaReplicatorBase()
    , mOverlays()
    , mActiveOverlaysHistory()
    , mCurrentOpenedOverlay()
    , mSceneWp(scene)
{
}

void OverlayManager::RegisterOverlay(std::shared_ptr<IUiOverlay> overlay)
{
    const auto& foundOverlay = FindOverlay(overlay->GetOverlayName());
    assert(!foundOverlay);
    mOverlays.emplace_back(overlay);
    overlay->SubscribeOnAnimationFinished([weak = weak_from_this()](const std::string& overlayName) {
        if (const auto& overlayManagerPtr = weak.lock()) {
            const auto overlayPtr = std::static_pointer_cast<OverlayManager>(overlayManagerPtr);
            if (overlayPtr->GetPendingAnimationFinishesToOpenOverlay() && overlayName == "FadeOut") {
                overlayPtr->SetPendingAnimationFinishesToOpenOverlay(false);
                assert(overlayPtr->GetCurrentOpenedOverlay());
                overlayPtr->GetCurrentOpenedOverlay()->OpenOverlay();
            }
        }
    });
}

void OverlayManager::RegisterBackgroundOverlay(std::shared_ptr<IUiOverlay> overlay)
{
    const auto& foundOverlay = FindBackgroundOverlay(overlay->GetOverlayName());
    assert(!foundOverlay);
    mBackgroundOverlays.emplace_back(overlay);
}

void OverlayManager::UnregisterOverlay(std::shared_ptr<IUiOverlay> overlay)
{
    const auto& foundOverlay = FindOverlay(overlay->GetOverlayName());
    assert(foundOverlay);
    auto remove_it = std::remove_if(mOverlays.begin(), mOverlays.end(), [&](const auto& myOverlay) {
        return overlay->GetOverlayName() == myOverlay->GetOverlayName();
    });
    mOverlays.erase(remove_it);
}

bool OverlayManager::GetPendingAnimationFinishesToOpenOverlay() const
{
    return mPendingAnimationFinishesToOpenOverlay;
}

void OverlayManager::SetPendingAnimationFinishesToOpenOverlay(const bool value)
{
    mPendingAnimationFinishesToOpenOverlay = value;
}

std::shared_ptr<IUiOverlay> OverlayManager::GetCurrentOpenedOverlay() const
{
    return mCurrentOpenedOverlay;
}

void OverlayManager::UnregisterBackgroundOverlay(std::shared_ptr<IUiOverlay> overlay)
{
    const auto& foundOverlay = FindBackgroundOverlay(overlay->GetOverlayName());
    assert(foundOverlay);
    auto remove_it = std::remove_if(mBackgroundOverlays.begin(), mBackgroundOverlays.end(), [&](const auto& myOverlay) {
        return overlay->GetOverlayName() == myOverlay->GetOverlayName();
    });
    mBackgroundOverlays.erase(remove_it);
}

std::shared_ptr<LuaProxy> OverlayManager::ReplicateLuaProxy()
{
    return std::make_shared<OverlayManagerLuaProxy>(std::static_pointer_cast<OverlayManager>(shared_from_this()));
}

void OverlayManager::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
}

std::shared_ptr<IUiOverlay> OverlayManager::GetOverlayByName(const std::string& name)
{
    return FindOverlay(name);
}

std::weak_ptr<::EngineCore::Scene> OverlayManager::GetSceneWp() const
{
    return mSceneWp;
}

void OverlayManager::OpenOverlay(const std::string& overlayName, const bool isRestoreFromHistory)
{
    const auto& foundOverlay = FindOverlay(overlayName);
    assert(foundOverlay);

    if (mCurrentOpenedOverlay) {
        if (mCurrentOpenedOverlay->GetOverlayName() == overlayName) {
            LogInfo("OverlayManager::OpenOverlay: Attempt to open already opened overlay ", overlayName);
            return;
        }
        if (mCurrentOpenedOverlay->HasFadeOutAnimation()) {
            mPendingAnimationFinishesToOpenOverlay = true;
        }
        mCurrentOpenedOverlay->CloseOverlay();
        LogInfo("OverlayManager::OpenOverlay: store to history ", mCurrentOpenedOverlay->GetOverlayName());

        if (!isRestoreFromHistory) {
            mActiveOverlaysHistory.push(
                mCurrentOpenedOverlay->GetOverlayName()); // store current overlay to stack, to be able to restore this overlay
        }
    }
    mCurrentOpenedOverlay = foundOverlay;
    if (!mPendingAnimationFinishesToOpenOverlay) {
        mCurrentOpenedOverlay->OpenOverlay();
    }
    SyncLuaThreadData();
}

void OverlayManager::OpenBackgroundOverlay(const std::string& overlayName)
{
    const auto& foundOverlay = FindBackgroundOverlay(overlayName);
    assert(foundOverlay);
    if (!mActiveBackgroundOverlays.count(overlayName)) {
        mActiveBackgroundOverlays.emplace(overlayName);
        foundOverlay->OpenOverlay();
        SyncLuaThreadData();
    }
}

void OverlayManager::CloseBackgroundOverlay(const std::string& overlayName)
{
    if (mActiveBackgroundOverlays.count(overlayName)) {
        const auto& foundOverlay = FindBackgroundOverlay(overlayName);
        assert(foundOverlay);
        mActiveBackgroundOverlays.erase(overlayName);
        foundOverlay->CloseOverlay();
        SyncLuaThreadData();
    }
}

void OverlayManager::CloseCurrentOverlay()
{
    if (mCurrentOpenedOverlay) {
        mCurrentOpenedOverlay->CloseOverlay();

        if (mActiveOverlaysHistory.size()) {
            // Restore previously opened overlay
            const auto currentActiveOverlayName = mActiveOverlaysHistory.top();
            mActiveOverlaysHistory.pop();
            OpenOverlay(currentActiveOverlayName, true);
            LogInfo("OverlayManager::CloseCurrentOverlay: remove from history ", currentActiveOverlayName);
        } else {
            // History is empty, nothing to restore
            mCurrentOpenedOverlay = nullptr;
            SyncLuaThreadData();
        }
    }
}

void OverlayManager::CloseOverlayAndClearHistory()
{
    if (mCurrentOpenedOverlay) {
        mCurrentOpenedOverlay->CloseOverlay();
        mCurrentOpenedOverlay = nullptr;
        std::stack<std::string>().swap(mActiveOverlaysHistory);
        SyncLuaThreadData();
    }
}

bool OverlayManager::CurrentOverlayOpened() const
{
    return mCurrentOpenedOverlay != nullptr;
}

std::string OverlayManager::GetCurrentOpenedOverlayName() const
{
    return mCurrentOpenedOverlay ? mCurrentOpenedOverlay->GetOverlayName() : "";
}

void OverlayManager::Tick(const float deltaTime)
{
    for (const auto& overlay : mOverlays) {
        overlay->Tick(deltaTime);
    }
}

void OverlayManager::UnpausableTick(const float deltaTime)
{
    for (const auto& overlay : mOverlays) {
        overlay->UnpausableTick(deltaTime);
    }
}

void OverlayManager::Initialize()
{
    static constexpr auto functionId = Hash64_CT("OverlayManager::Initialize");

    if (const auto& sceneSp = mSceneWp.lock()) {
        sceneSp->RegisterEngineToLuaReplicator(shared_from_this());
        SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
        const auto& overlayManagerLuaProxy = std::static_pointer_cast<OverlayManagerLuaProxy>(ReplicateLuaProxy());
        overlayManagerLuaProxy->SetSceneWp(sceneSp);
        overlayManagerLuaProxy->SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
        if (const auto& luaProcessorSp = mLuaScriptProcessorWp.lock()) {
            luaProcessorSp->SetOverlayManagerLuaProxy(overlayManagerLuaProxy);
        }

        for (const auto& overlay : mOverlays) {
            overlay->Initialize();
        }
    }
}

std::shared_ptr<IUiOverlay> OverlayManager::FindOverlay(const std::string& overlayName) const
{
    const auto findIt = std::find_if(
        mOverlays.cbegin(), mOverlays.cend(), [&](const auto& overlay) { return overlay->GetOverlayName() == overlayName; });
    return findIt != mOverlays.end() ? *findIt : nullptr;
}

std::shared_ptr<IUiOverlay> OverlayManager::FindBackgroundOverlay(const std::string& overlayName) const
{
    const auto findIt = std::find_if(mBackgroundOverlays.cbegin(), mBackgroundOverlays.cend(), [&](const auto& overlay) {
        return overlay->GetOverlayName() == overlayName;
    });
    return findIt != mBackgroundOverlays.end() ? *findIt : nullptr;
}

void OverlayManager::SyncLuaThreadData()
{
    if (const auto& sceneSp = mSceneWp.lock()) {
        if (const auto& luaProcessorSp = mLuaScriptProcessorWp.lock()) {
            if (const auto& overlayManagerLuaProxy = luaProcessorSp->GetOverlayManagerLuaProxy()) {
                static constexpr auto functionId = Hash64_CT("OverlayManager::SyncLuaThreadData");
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetReplicatorId(),
                    functionId,
                    [overlayManagerLuaProxy,
                     overlayName = GetCurrentOpenedOverlayName(),
                     overlayNames = mActiveBackgroundOverlays]() {
                        overlayManagerLuaProxy->SetCurrentOverlay(overlayName);
                        overlayManagerLuaProxy->SetActiveBackgroundOverlays(overlayNames);
                    });
            }
        }
    }
}

void OverlayManager::CleanUp()
{
    for (const auto& overlay : mOverlays) {
        overlay->CleanUp();
    }
    for (const auto& backgroundOverlay : mBackgroundOverlays) {
        backgroundOverlay->CleanUp();
    }
    mOverlays.clear();
    mBackgroundOverlays.clear();
}
} // namespace GUI
} // namespace EngineCore
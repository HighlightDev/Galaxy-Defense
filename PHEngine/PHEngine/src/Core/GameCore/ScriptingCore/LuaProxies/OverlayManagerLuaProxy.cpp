#include "OverlayManagerLuaProxy.h"

#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore::GUI;
using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
OverlayManagerLuaProxy::OverlayManagerLuaProxy(const std::shared_ptr<OverlayManager>& owner)
    : LuaProxy()
    , mCurrentOverlayName()
    , mActiveBackgroundOverlays()
{
    mLuaProxyId = CreateUniqueLuaProxyId();
    SetReplicatorId(owner->GetReplicatorId());
    owner->SetLuaProxyId(mLuaProxyId);
}

void OverlayManagerLuaProxy::CleanUp()
{
    mCurrentOverlayName = "";
    mActiveBackgroundOverlays.clear();
}

void OverlayManagerLuaProxy::SetCurrentOverlay(const std::string& currentOverlayName)
{
    mCurrentOverlayName = currentOverlayName;
}

void OverlayManagerLuaProxy::SetActiveBackgroundOverlays(const std::unordered_set<std::string>& backgroundOverlays)
{
    mActiveBackgroundOverlays = backgroundOverlays;
}

std::string OverlayManagerLuaProxy::GetCurrentOverlayName() const
{
    return mCurrentOverlayName;
}

void OverlayManagerLuaProxy::OpenOverlay(const std::string& overlayName)
{
    static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::OpenOverlay");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId, overlayName](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                ext_assert(replicator, "OverlayManagerLuaProxy::OpenOverlay: replicator is null");
                const auto& overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                ext_assert(overlayManager, "OverlayManagerLuaProxy::OpenOverlay: overlayManager is null");
                overlayManager->OpenOverlay(overlayName);
            });
    }
}

void OverlayManagerLuaProxy::OpenBackgroundOverlay(const std::string& overlayName)
{
    static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::OpenBackgroundOverlay");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId, overlayName](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                ext_assert(replicator, "OverlayManagerLuaProxy::OpenBackgroundOverlay: replicator is null");
                const auto& overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                ext_assert(overlayManager, "OverlayManagerLuaProxy::OpenBackgroundOverlay: overlayManager is null");
                overlayManager->OpenBackgroundOverlay(overlayName);
            });
    }
}

void OverlayManagerLuaProxy::CloseCurrentOverlay()
{
    static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::CloseCurrentOverlay");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                ext_assert(replicator, "OverlayManagerLuaProxy::CloseCurrentOverlay: replicator is null");
                const auto& overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                ext_assert(overlayManager, "OverlayManagerLuaProxy::CloseCurrentOverlay: overlayManager is null");
                overlayManager->CloseCurrentOverlay();
            });
    }
}

void OverlayManagerLuaProxy::CloseOverlayAndClearHistory()
{
    static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::CloseOverlayAndClearHistory");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                ext_assert(replicator, "OverlayManagerLuaProxy::CloseOverlayAndClearHistory: replicator is null");
                const auto& overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                ext_assert(overlayManager, "OverlayManagerLuaProxy::CloseOverlayAndClearHistory: overlayManager is null");
                overlayManager->CloseOverlayAndClearHistory();
            });
    }
}

void OverlayManagerLuaProxy::CloseBackgroundOverlay(const std::string& overlayName)
{
    static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::CloseBackgroundOverlay");
    if (const auto sceneSp = mSceneWp.lock()) {
        const auto replicatorId = GetReplicatorId();
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            mLuaProxyId,
            functionId,
            [sceneSp, replicatorId, overlayName](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                const auto& replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                ext_assert(replicator, "OverlayManagerLuaProxy::CloseBackgroundOverlay: replicator is null");
                const auto& overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                ext_assert(overlayManager, "OverlayManagerLuaProxy::CloseBackgroundOverlay: overlayManager is null");
                overlayManager->CloseBackgroundOverlay(overlayName);
            });
    }
}

void OverlayManagerLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
}

std::string OverlayManagerLuaProxy::GetGameThreadData()
{
    return "";
}
} // namespace Scripts
} // namespace EngineCore

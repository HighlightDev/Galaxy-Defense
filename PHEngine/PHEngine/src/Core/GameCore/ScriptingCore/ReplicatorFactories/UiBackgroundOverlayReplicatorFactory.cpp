#include "UiBackgroundOverlayReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"
#include "Core/GameCore/GUI/OverlayManagement/UiOverlay.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore {
namespace Scripts {
int32_t UiBackgroundOverlayReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"),
        "UiBackgroundOverlayReplicatorFactory::CreateReplicator: Not called from Lua thread");
    const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
    const auto canvasLuaProxyId = jsonObj["canvasLuaProxyId"].get<int32_t>();
    const auto& overlayName = jsonObj["overlayName"].get<std::string>();

    const auto overlayLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();
    const auto luaScriptProcessorSp = luaScriptProcessorWp.lock();
    ext_assert(
        luaScriptProcessorSp, "UiBackgroundOverlayReplicatorFactory::CreateReplicator: LuaScriptProcessor weak_ptr lock failed");
    const auto overlayManagerReplicatorId = luaScriptProcessorSp->GetOverlayManagerLuaProxy()->GetReplicatorId();

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiBackgroundOverlayReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            canvasLuaProxyId,
            functionId,
            [sceneSp, luaScriptProcessorWp, overlayManagerReplicatorId, overlayName, canvasLuaProxyId, overlayLuaProxyId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                ext_assert(
                    ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Game"),
                    "UiBackgroundOverlayReplicatorFactory::CreateReplicator: Not called from Game thread");
                const auto& uiOverlay = std::make_shared<UiOverlay>(overlayName, sceneSp, luaScriptProcessorWp);
                const auto& uiCanvas = std::static_pointer_cast<::EngineCore::GUI::UiCanvas>(
                    sceneSp->GetEngineToLuaReplicatorByLuaProxyId(canvasLuaProxyId));
                const auto& overlayManager
                    = std::static_pointer_cast<OverlayManager>(sceneSp->GetEngineToLuaReplicatorById(overlayManagerReplicatorId));
                ext_assert(uiCanvas, "UiBackgroundOverlayReplicatorFactory::CreateReplicator: uiCanvas is null");
                uiOverlay->SetOverlayCanvas(uiCanvas);
                uiOverlay->SetLuaProxyId(overlayLuaProxyId);
                uiOverlay->SetLuaScriptProcessor(luaScriptProcessorWp);
                overlayManager->RegisterBackgroundOverlay(uiOverlay);
                sceneSp->RegisterEngineToLuaReplicator(uiOverlay);
                uiOverlay->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiBackgroundOverlayReplicatorFactory::CreateReplicator: Scene weak_ptr lock failed");
        return -1;
    }

    return overlayLuaProxyId;
}
} // namespace Scripts
} // namespace EngineCore
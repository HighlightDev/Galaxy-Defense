#include "UiCanvasReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore {
namespace Scripts {
int32_t UiCanvasReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));
    const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
    const auto& originX = jsonObj["originX"].get<int32_t>();
    const auto& originY = jsonObj["originY"].get<int32_t>();
    const auto& width = jsonObj["width"].get<int32_t>();
    const auto& height = jsonObj["height"].get<int32_t>();
    std::string name = "";
    if (jsonObj.contains("name")) {
        name = jsonObj["name"].get<std::string>();
    }

    const auto canvasLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiCanvasReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            canvasLuaProxyId,
            functionId,
            [originX, originY, width, height, sceneSp, luaScriptProcessorWp, canvasLuaProxyId, name = name](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Game"));
                const auto& createdUiCanvas
                    = sceneSp->GetUiHandler()->CreateCanvas(ViewPortInfo(originX, originY, width, height), name);
                sceneSp->RegisterEngineToLuaReplicator(createdUiCanvas);
                createdUiCanvas->SetIsVisible(false);
                createdUiCanvas->SetLuaProxyId(canvasLuaProxyId);
                createdUiCanvas->SetLuaScriptProcessor(luaScriptProcessorWp);
                createdUiCanvas->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiCanvasReplicatorFactory::CreateReplicator => Scene weak_ptr lock failed");
        return -1;
    }

    return canvasLuaProxyId;
}
} // namespace Scripts
} // namespace EngineCore
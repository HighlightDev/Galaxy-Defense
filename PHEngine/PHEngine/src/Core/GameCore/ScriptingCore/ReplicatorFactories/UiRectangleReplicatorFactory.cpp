#include "UiRectangleReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore {
namespace Scripts {
int32_t UiRectangleReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));
    const auto uiRectangleLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    std::string name = "";
    if (jsonParamsStr != "") {
        const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
        if (jsonObj.contains("name")) {
            name = jsonObj["name"].get<std::string>();
        }
    }

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiRectangleReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            uiRectangleLuaProxyId,
            functionId,
            [sceneSp, luaScriptProcessorWp, uiRectangleLuaProxyId, name](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Game"));
                const auto& createdUiRectangle = std::make_shared<UiRectangle>(name);
                createdUiRectangle->Initialize();
                createdUiRectangle->SetLuaProxyId(uiRectangleLuaProxyId);
                createdUiRectangle->SetLuaScriptProcessor(luaScriptProcessorWp);
                sceneSp->RegisterEngineToLuaReplicator(createdUiRectangle);
                createdUiRectangle->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiRectangleReplicatorFactory::CreateReplicator => Scene weak_ptr lock failed");
        return -1;
    }

    return uiRectangleLuaProxyId;
}
} // namespace Scripts
} // namespace EngineCore
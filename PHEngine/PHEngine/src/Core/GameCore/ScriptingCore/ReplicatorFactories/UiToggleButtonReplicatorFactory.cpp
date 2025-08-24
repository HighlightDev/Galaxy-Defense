#include "UiToggleButtonReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/UiToggleButton.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore {
namespace Scripts {
int32_t UiToggleButtonReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));
    const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
    const auto isStateOn = jsonObj["is_state_on"].get<bool>();
    std::string name = "";
    if (jsonObj.contains("name")) {
        name = jsonObj["name"].get<std::string>();
    }

    const auto uiToggleButtonLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiToggleButtonReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            uiToggleButtonLuaProxyId,
            functionId,
            [sceneSp, isStateOn, luaScriptProcessorWp, uiToggleButtonLuaProxyId, name](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Game"));
                const auto& createdUiToggleButton = std::make_shared<UiToggleButton>(isStateOn, name);
                createdUiToggleButton->Initialize();
                createdUiToggleButton->SetLuaProxyId(uiToggleButtonLuaProxyId);
                createdUiToggleButton->SetLuaScriptProcessor(luaScriptProcessorWp);
                sceneSp->RegisterEngineToLuaReplicator(createdUiToggleButton);
                createdUiToggleButton->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiToggleButtonReplicatorFactory::CreateReplicator => Scene weak_ptr lock failed");
        return -1;
    }

    return uiToggleButtonLuaProxyId;
}
} // namespace Scripts
} // namespace EngineCore
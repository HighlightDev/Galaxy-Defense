#include "UiLabelReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/EngineConstants.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/UiLabel.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore {
namespace Scripts {
int32_t UiLabelReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_luaThreadName),
        "UiLabelReplicatorFactory::CreateReplicator: Not called from Lua thread");
    const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
    const auto& fontName = jsonObj["font_name"].get<std::string>();
    std::string name = "";
    if (jsonObj.contains("name")) {
        name = jsonObj["name"].get<std::string>();
    }
    const auto uiLabelLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiLabelReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            uiLabelLuaProxyId,
            functionId,
            [sceneSp, fontName, luaScriptProcessorWp, uiLabelLuaProxyId, name](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                ext_assert(
                    ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_gameThreadName),
                    "UiLabelReplicatorFactory::CreateReplicator: Not called from Game thread");
                const auto& createdUiLabel = std::make_shared<UiLabel>(fontName, name);
                createdUiLabel->Initialize();
                createdUiLabel->SetLuaProxyId(uiLabelLuaProxyId);
                createdUiLabel->SetLuaScriptProcessor(luaScriptProcessorWp);
                sceneSp->RegisterEngineToLuaReplicator(createdUiLabel);
                createdUiLabel->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiLabelReplicatorFactory::CreateReplicator: Scene weak_ptr lock failed");
        return -1;
    }

    return uiLabelLuaProxyId;
}
} // namespace Scripts
} // namespace EngineCore
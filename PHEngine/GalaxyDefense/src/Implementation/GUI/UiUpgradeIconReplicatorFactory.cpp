#include "UiUpgradeIconReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/EngineConstants.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Implementation/GUI/UiUpgradeIcon.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace EngineCore::Scripts;

namespace Game {

int32_t UiUpgradeIconReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_luaThreadName),
        "UiUpgradeIconReplicatorFactory::CreateReplicator: Not called from Lua thread");
    const auto uiUpgradeIconLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    std::string name = "";
    if (jsonParamsStr != "") {
        const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
        if (jsonObj.contains("name")) {
            name = jsonObj["name"].get<std::string>();
        }
    }

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiUpgradeIconReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            uiUpgradeIconLuaProxyId,
            functionId,
            [sceneSp, luaScriptProcessorWp, uiUpgradeIconLuaProxyId, name](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                ext_assert(
                    ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_gameThreadName),
                    "UiUpgradeIconReplicatorFactory::CreateReplicator: Not called from Game thread");
                const auto& createdUiUpgradeIcon = std::make_shared<UiUpgradeIcon>(name);
                createdUiUpgradeIcon->Initialize();
                createdUiUpgradeIcon->SetLuaProxyId(uiUpgradeIconLuaProxyId);
                createdUiUpgradeIcon->SetLuaScriptProcessor(luaScriptProcessorWp);
                sceneSp->RegisterEngineToLuaReplicator(createdUiUpgradeIcon);
                createdUiUpgradeIcon->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiUpgradeIconReplicatorFactory::CreateReplicator: Scene weak_ptr lock failed");
        return -1;
    }

    return uiUpgradeIconLuaProxyId;
}

} // namespace Game

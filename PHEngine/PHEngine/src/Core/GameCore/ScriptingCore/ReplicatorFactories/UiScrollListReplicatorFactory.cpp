#include "UiScrollListReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/EngineConstants.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/UiScrollList.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {

int32_t UiScrollListReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_luaThreadName),
        "UiScrollListReplicatorFactory::CreateReplicator: Not called from Lua thread");

    const auto uiItemLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    std::string name;
    if (!jsonParamsStr.empty()) {
        const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
        if (jsonObj.contains("name")) {
            name = jsonObj["name"].get<std::string>();
        }
    }

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiScrollListReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            uiItemLuaProxyId,
            functionId,
            [sceneSp, luaScriptProcessorWp, uiItemLuaProxyId, name](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                ext_assert(
                    ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_gameThreadName),
                    "UiScrollListReplicatorFactory::CreateReplicator: Not called from Game thread");
                const auto& createdUiItem = std::make_shared<UiScrollList>(name);
                createdUiItem->Initialize();
                createdUiItem->SetLuaProxyId(uiItemLuaProxyId);
                createdUiItem->SetLuaScriptProcessor(luaScriptProcessorWp);
                sceneSp->RegisterEngineToLuaReplicator(createdUiItem);
                createdUiItem->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiScrollListReplicatorFactory::CreateReplicator: Scene weak_ptr lock failed");
        return -1;
    }

    return uiItemLuaProxyId;
}

} // namespace Scripts
} // namespace EngineCore

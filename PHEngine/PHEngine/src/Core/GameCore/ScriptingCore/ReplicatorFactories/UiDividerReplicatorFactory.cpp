#include "UiDividerReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/EngineConstants.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/UiDIvider.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore {
namespace Scripts {
int32_t UiDividerReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_luaThreadName),
        "UiDividerReplicatorFactory::CreateReplicator: Not called from Lua thread");
    const auto uiDividerLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    std::string name = "";
    if (not jsonParamsStr.empty()) {
        const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
        if (jsonObj.contains("name")) {
            name = jsonObj["name"].get<std::string>();
        }
    }

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiDividerReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            uiDividerLuaProxyId,
            functionId,
            [sceneSp, luaScriptProcessorWp, uiDividerLuaProxyId, name](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                ext_assert(
                    ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName(EngineConstants::c_gameThreadName),
                    "UiDividerReplicatorFactory::CreateReplicator: Not called from Game thread");
                const auto& createdUiDivider = std::make_shared<UiDivider>(name);
                createdUiDivider->Initialize();
                createdUiDivider->SetLuaProxyId(uiDividerLuaProxyId);
                createdUiDivider->SetLuaScriptProcessor(luaScriptProcessorWp);
                sceneSp->RegisterEngineToLuaReplicator(createdUiDivider);
                createdUiDivider->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiDividerReplicatorFactory::CreateReplicator: Scene weak_ptr lock failed");
        return -1;
    }

    return uiDividerLuaProxyId;
}
} // namespace Scripts
} // namespace EngineCore
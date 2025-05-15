#include "UiScrollBarReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/UiScrollBar.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore::Scripts {
int32_t UiScrollBarReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));
    const auto uiScrollBarLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    std::string name = "";
    if (jsonParamsStr != "") {
        const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
        if (jsonObj.contains("name")) {
            name = jsonObj["name"].get<std::string>();
        }
    }

    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiScrollBarReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            uiScrollBarLuaProxyId,
            functionId,
            [sceneSp, luaScriptProcessorWp, uiScrollBarLuaProxyId, name]() {
                assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Game"));
                const auto& createdScrollBar = std::make_shared<UiScrollBar>(name);
                createdScrollBar->Initialize();
                createdScrollBar->SetLuaProxyId(uiScrollBarLuaProxyId);
                createdScrollBar->SetLuaScriptProcessor(luaScriptProcessorWp);
                sceneSp->RegisterEngineToLuaReplicator(createdScrollBar);
                createdScrollBar->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiScrollBarReplicatorFactory::CreateReplicator => Scene weak_ptr lock failed");
        return -1;
    }

    return uiScrollBarLuaProxyId;
}
} // namespace EngineCore::Scripts
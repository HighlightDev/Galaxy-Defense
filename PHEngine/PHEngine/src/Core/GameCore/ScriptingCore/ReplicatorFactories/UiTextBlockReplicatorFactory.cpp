#include "UiTextBlockReplicatorFactory.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/GUI/UiElements/UiTextBlock.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore {
namespace Scripts {
int32_t UiTextBlockReplicatorFactory::CreateReplicator(
    const std::weak_ptr<Scene>& sceneWp,
    const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp,
    const std::string& jsonParamsStr) const
{
    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));
    const auto uiTextBlockLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

    std::string name = "", fontName = "";
    if (jsonParamsStr != "") {
        const auto& jsonObj = nlohmann::json::parse(jsonParamsStr);
        if (jsonObj.contains("name")) {
            name = jsonObj["name"].get<std::string>();
        }
        if (jsonObj.contains("font_name")) {
            fontName = jsonObj["font_name"].get<std::string>();
        }
    }

    assert(fontName.size());
    if (const auto& sceneSp = sceneWp.lock()) {
        static constexpr auto functionId = Hash64_CT("UiTextBlockReplicatorFactory::CreateReplicator");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            uiTextBlockLuaProxyId,
            functionId,
            [sceneSp, luaScriptProcessorWp, uiTextBlockLuaProxyId, name, fontName]() {
                assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Game"));
                const auto& createdUiTextBlock = std::make_shared<UiTextBlock>(fontName, name);
                createdUiTextBlock->Initialize();
                createdUiTextBlock->SetLuaProxyId(uiTextBlockLuaProxyId);
                createdUiTextBlock->SetLuaScriptProcessor(luaScriptProcessorWp);
                sceneSp->RegisterEngineToLuaReplicator(createdUiTextBlock);
                createdUiTextBlock->SetPendingToCreateLuaProxy();
            });
    } else {
        LogInfo("UiTextBlockReplicatorFactory::CreateReplicator => Scene weak_ptr lock failed");
        return -1;
    }

    return uiTextBlockLuaProxyId;
}
} // namespace Scripts
} // namespace EngineCore
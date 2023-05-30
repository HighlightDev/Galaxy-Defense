#include "UiOverlayReplicatorFactory.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/OverlayManagement/UiOverlay.h"
#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore
{
    namespace Scripts
    {
        int32_t UiOverlayReplicatorFactory::CreateReplicator(const std::weak_ptr<Scene> &sceneWp,
                                                            const std::weak_ptr<LuaScriptProcessor> &luaScriptProcessorWp,
                                                            const std::string &jsonParamsStr) const
        {
            assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));
            const auto &jsonObj = nlohmann::json::parse(jsonParamsStr);
            const auto canvasLuaProxyId = jsonObj["canvasLuaProxyId"].get<int32_t>();
            const auto &overlayName = jsonObj["overlayName"].get<std::string>();

            const auto overlayLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();
            const auto luaScriptProcessorSp = luaScriptProcessorWp.lock();
            assert(luaScriptProcessorSp);
            const auto overlayManagerReplicatorId = luaScriptProcessorSp->GetOverlayManagerLuaProxy()->GetReplicatorId();

            if (const auto &sceneSp = sceneWp.lock())
            {
                static constexpr auto functionId = Hash64_CT("UiOverlayReplicatorFactory::CreateReplicator");
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, canvasLuaProxyId, functionId,
                [sceneSp, luaScriptProcessorWp, overlayManagerReplicatorId, overlayName, canvasLuaProxyId, overlayLuaProxyId]() {
                    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Game"));
                    const auto& uiOverlay = std::make_shared<UiOverlay>(overlayName, sceneSp, luaScriptProcessorWp);
                    const auto& uiCanvas = std::static_pointer_cast<::EngineCore::GUI::UiCanvas>(sceneSp->GetEngineToLuaReplicatorByLuaProxyId(canvasLuaProxyId));
                    const auto& overlayManager = std::static_pointer_cast<OverlayManager>(sceneSp->GetEngineToLuaReplicatorById(overlayManagerReplicatorId));
                    assert(uiCanvas);
                    uiOverlay->SetOverlayCanvas(uiCanvas);
                    uiOverlay->SetLuaProxyId(overlayLuaProxyId);
                    uiOverlay->SetLuaScriptProcessor(luaScriptProcessorWp);
                    overlayManager->RegisterOverlay(uiOverlay);
                    sceneSp->RegisterEngineToLuaReplicator(uiOverlay);
                    uiOverlay->SetPendingToCreateLuaProxy();
                });
            }
            else
            {
                LogInfo("UiOverlayReplicatorFactory::CreateReplicator => Scene weak_ptr lock failed");
                return -1;
            }

            return overlayLuaProxyId;
        }
    }
} // namespace EngineCore
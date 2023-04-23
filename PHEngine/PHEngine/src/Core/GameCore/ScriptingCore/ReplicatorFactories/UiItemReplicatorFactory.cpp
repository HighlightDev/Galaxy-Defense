#include "UiItemReplicatorFactory.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GameCore/GUI/UiElements/UiItem.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace Graphics;

namespace EngineCore
{
    namespace Scripts
    {
        int32_t UiItemReplicatorFactory::CreateReplicator(const std::weak_ptr<Scene> &sceneWp,
                                                            const std::weak_ptr<LuaScriptProcessor> &luaScriptProcessorWp,
                                                            const std::string &jsonParamsStr) const
        {
            assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Lua"));
            const auto uiItemLuaProxyId = LuaProxy::CreateUniqueLuaProxyId();

            if (const auto &sceneSp = sceneWp.lock())
            {
                static constexpr auto functionId = Hash64_CT("UiItemReplicatorFactory::CreateReplicator");
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, uiItemLuaProxyId, functionId, [sceneSp, luaScriptProcessorWp, uiItemLuaProxyId]() {
                    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Game"));
                    const auto& createdUiItem = std::make_shared<UiItem>();
                    createdUiItem->SetLuaProxyId(uiItemLuaProxyId);
                    createdUiItem->SetLuaScriptProcessor(luaScriptProcessorWp);
                    sceneSp->RegisterEngineToLuaReplicator(createdUiItem);
                    const auto& uiItemLuaProxy = createdUiItem->ReplicateLuaProxy();
                    uiItemLuaProxy->SetSceneWp(sceneSp);
                    uiItemLuaProxy->SetLuaScriptProcessor(luaScriptProcessorWp);

                    if (const auto& luaProcessorSp = luaScriptProcessorWp.lock())
                    {
                        luaProcessorSp->AddLuaProxy(uiItemLuaProxy);
                    }
                });
            }
            else
            {
                LogInfo("UiItemReplicatorFactory::CreateReplicator => Scene weak_ptr lock failed");
                return -1;
            }

            return uiItemLuaProxyId;
        }
    }
} // namespace EngineCore
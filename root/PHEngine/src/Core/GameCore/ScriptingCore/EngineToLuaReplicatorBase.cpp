#include "EngineToLuaReplicatorBase.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
int32_t EngineToLuaReplicatorBase::s_replicatorId = -1;
EngineToLuaReplicatorBase::EngineToLuaReplicatorBase()
    : mReplicatorId(++s_replicatorId)
    , mLuaProxyId(-1)
    , mLuaScriptProcessorWp()
{
}

int32_t EngineToLuaReplicatorBase::GetReplicatorId() const
{
    return mReplicatorId;
}

void EngineToLuaReplicatorBase::SetLuaProxyId(const int32_t luaProxyId)
{
    mLuaProxyId = luaProxyId;
}

int32_t EngineToLuaReplicatorBase::GetLuaProxyId() const
{
    return mLuaProxyId;
}

void EngineToLuaReplicatorBase::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessor)
{
    mLuaScriptProcessorWp = luaScriptProcessor;
}

std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> EngineToLuaReplicatorBase::GetLuaScriptProcessorWp() const
{
    return mLuaScriptProcessorWp;
}

void EngineToLuaReplicatorBase::SetPendingToCreateLuaProxy()
{
    mIsPendingToAddLuaProxy = true;
}

bool EngineToLuaReplicatorBase::GetIsPendingToCreateLuaProxy() const
{
    return mIsPendingToAddLuaProxy;
}

void EngineToLuaReplicatorBase::InitLuaProxy(const std::shared_ptr<Scene>& sceneSp)
{
    static constexpr uint64_t functionId = Hash64_CT("EngineToLuaReplicatorBase::InitLuaProxy");
    if (const auto& luaScriptProcessorSp = GetLuaScriptProcessorWp().lock()) {
        mIsPendingToAddLuaProxy = false;
        const auto& luaProxy = ReplicateLuaProxy();
        luaProxy->SetSceneWp(sceneSp);
        luaProxy->SetLuaScriptProcessor(luaScriptProcessorSp);
        luaScriptProcessorSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
            eEnqueueJobPolicy::PUSH_ANYWAY,
            GetReplicatorId(),
            functionId,
            [this, luaScriptProcessorSp, luaProxy](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                luaScriptProcessorSp->AddLuaProxy(luaProxy);
            });
    }
}
} // namespace Scripts
} // namespace EngineCore

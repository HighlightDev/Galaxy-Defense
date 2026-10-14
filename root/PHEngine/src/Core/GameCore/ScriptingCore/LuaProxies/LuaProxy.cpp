#include "LuaProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
std::atomic<int32_t> LuaProxy::s_LuaProxyId = -1;

LuaProxy::LuaProxy()
    : mLuaProxyId(-1)
    , mReplicatorId(-1)
    , mIsLuaDataDirty(true)
{
}

void LuaProxy::CleanUp()
{
}

bool LuaProxy::IsLuaDataDirty() const
{
    return mIsLuaDataDirty;
}

int32_t LuaProxy::GetLuaProxyId() const
{
    ext_assert(mLuaProxyId != -1, "LuaProxy::GetLuaProxyId: mLuaProxyId is invalid");
    return mLuaProxyId;
}

int32_t LuaProxy::GetReplicatorId() const
{
    ext_assert(mReplicatorId != -1, "LuaProxy::GetReplicatorId: mReplicatorId is invalid");
    return mReplicatorId;
}

void LuaProxy::SetReplicatorId(const int32_t id)
{
    mReplicatorId = id;
}

void LuaProxy::SetSceneWp(const std::weak_ptr<::EngineCore::Scene>& sceneWp)
{
    mSceneWp = sceneWp;
}

void LuaProxy::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp)
{
    mLuaScriptProcessorWp = luaScriptProcessorWp;
}

int32_t LuaProxy::CreateUniqueLuaProxyId()
{
    return ++s_LuaProxyId;
}
} // namespace Scripts
} // namespace EngineCore
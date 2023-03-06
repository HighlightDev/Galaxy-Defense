#include "LuaProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore;

namespace EngineCore
{
    namespace Scripts
    {
        int32_t LuaProxy::s_LuaProxyId = -1;

        LuaProxy::LuaProxy()
            : mLuaProxyId(++s_LuaProxyId),
              mReplicatorId()
        {
        }

        int32_t LuaProxy::GetLuaProxyId() const
        {
            return mLuaProxyId;
        }

        int32_t LuaProxy::GetReplicatorId() const
        {
            return mReplicatorId;
        }

        void LuaProxy::SetReplicatorId(const int32_t id)
        {
            mReplicatorId = id;
        }

        void LuaProxy::SetSceneWp(const std::weak_ptr<::EngineCore::Scene> &sceneWp)
        {
            mSceneWp = sceneWp;
        }

        void LuaProxy::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &luaScriptProcessorWp)
        {
            mLuaScriptProcessorWp = luaScriptProcessorWp;
        }
    }
}
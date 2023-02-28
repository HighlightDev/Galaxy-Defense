#include "EngineToLuaReplicatorBase.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

namespace EngineCore
{
    namespace Scripts
    {
        int32_t EngineToLuaReplicatorBase::s_replicatorId = -1;
        EngineToLuaReplicatorBase::EngineToLuaReplicatorBase()
            : mReplicatorId(++s_replicatorId),
              mLuaProxyId(-1),
              mLuaScriptProcessorWp()
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

        void EngineToLuaReplicatorBase::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &luaScriptProcessor)
        {
            mLuaScriptProcessorWp = luaScriptProcessor;
        }
    }
}

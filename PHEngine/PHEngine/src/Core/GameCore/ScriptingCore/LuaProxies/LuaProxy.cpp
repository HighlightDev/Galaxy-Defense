#include "LuaProxy.h"

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
    }
}
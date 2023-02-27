#include "LuaProxy.h"

namespace EngineCore
{
    namespace Scripts
    {
        size_t LuaProxy::s_LuaProxyIdCounter = 0;

        LuaProxy::LuaProxy()
            : mLuaProxyId(s_LuaProxyIdCounter++),
              mGameObjectId()
        {
        }

        size_t LuaProxy::GetLuaProxyId() const
        {
            return mLuaProxyId;
        }

        size_t LuaProxy::GetGameObjectId() const
        {
            return mGameObjectId;
        }

        void LuaProxy::SetBindedGameObjectId(const uint64_t gameObjectId)
        {
            mGameObjectId = gameObjectId;
        }
    }
}
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <cstdint>

namespace EngineCore
{
    namespace Scripts
    {
        class LuaProxy
        {
            static size_t s_LuaProxyIdCounter;

        protected:
            size_t mLuaProxyId;

            uint64_t mGameObjectId;

        public:

            LuaProxy();

            virtual ~LuaProxy() = default;

            size_t GetLuaProxyId() const;

            uint64_t GetGameObjectId() const;

            void SetBindedGameObjectId(const uint64_t gameObjectId);
        };
    }
}
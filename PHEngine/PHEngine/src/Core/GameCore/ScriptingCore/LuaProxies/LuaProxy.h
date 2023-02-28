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
            static int32_t s_LuaProxyId;

        protected:
            int32_t mLuaProxyId;

            int32_t mReplicatorId;

        public:
            LuaProxy();

            virtual ~LuaProxy() = default;

            int32_t GetLuaProxyId() const;

            int32_t GetReplicatorId() const;

            void SetReplicatorId(const int32_t id);
        };
    }
}
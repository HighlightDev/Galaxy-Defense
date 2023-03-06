#pragma once

#include <stdint.h>
#include <stddef.h>
#include <cstdint>
#include <memory>

namespace EngineCore
{
    class Scene;
}

namespace EngineCore
{
    namespace Scripts
    {
        class LuaScriptProcessor;

        class LuaProxy
        {
            static int32_t s_LuaProxyId;

        protected:
            int32_t mLuaProxyId;

            int32_t mReplicatorId;

            std::weak_ptr<::EngineCore::Scene> mSceneWp;

            std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessorWp;

        public:
            LuaProxy();

            virtual ~LuaProxy() = default;

            int32_t GetLuaProxyId() const;

            int32_t GetReplicatorId() const;

            void SetReplicatorId(const int32_t id);

            void SetSceneWp(const std::weak_ptr<::EngineCore::Scene>& sceneWp);

            void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& luaScriptProcessorWp);
        };
    }
}
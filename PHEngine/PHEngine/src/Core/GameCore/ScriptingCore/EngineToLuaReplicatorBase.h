#pragma once

#include <memory>

namespace EngineCore
{
    namespace Scripts
    {
        class LuaProxy;
        class LuaScriptProcessor;

        class EngineToLuaReplicatorBase
            : public std::enable_shared_from_this<EngineToLuaReplicatorBase>
        {
            static int32_t s_replicatorId;

            int32_t mReplicatorId;

            int32_t mLuaProxyId;

        protected:
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> mLuaScriptProcessorWp;

        public:
            EngineToLuaReplicatorBase();

            virtual std::shared_ptr<LuaProxy> ReplicateLuaProxy() = 0;

            virtual void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) = 0; 

            int32_t GetReplicatorId() const;

            void SetLuaProxyId(const int32_t luaProxyId);

            int32_t GetLuaProxyId() const;

            void SetLuaScriptProcessor(const std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> &luaScriptProcessor);

            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> GetLuaScriptProcessorWp() const;
        };
    }
}

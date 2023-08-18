#pragma once

#include <stdint.h>
#include <stddef.h>
#include <cstdint>
#include <memory>
#include <atomic>
#include <string>

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
            static std::atomic<int32_t> s_LuaProxyId;

        protected:
            int32_t mLuaProxyId;

            int32_t mReplicatorId;

            std::weak_ptr<::EngineCore::Scene> mSceneWp;

            std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessorWp;

            bool mIsLuaDataDirty;

        public:
            LuaProxy();

            virtual ~LuaProxy() = default;

            virtual void CleanUp();

            virtual void OnLuaThreadDataUpdated(const std::string &jsonParameters) = 0;

            virtual std::string GetGameThreadData() = 0;

            bool IsLuaDataDirty() const;

            int32_t GetLuaProxyId() const;

            int32_t GetReplicatorId() const;

            void SetReplicatorId(const int32_t id);

            void SetSceneWp(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

            void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &luaScriptProcessorWp);

            static int32_t CreateUniqueLuaProxyId();
        };
    }
}
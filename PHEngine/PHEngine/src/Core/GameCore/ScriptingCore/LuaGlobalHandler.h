#pragma once

#include "LuaScriptExecutorBase.h"

#include <unordered_map>
#include <memory>

namespace EngineCore
{
    namespace Scripts
    {
        class LuaGlobalHandler
        {
            std::unordered_map<size_t, std::shared_ptr<LuaScriptExecutorBase>> mRegisteredLuaScriptExecutors;

        public:
            static LuaGlobalHandler *GetInstance()
            {
                static LuaGlobalHandler luaHandler;
                return &luaHandler;
            }

            LuaGlobalHandler();

            void RegisterLuaScriptExecutor(const std::shared_ptr<LuaScriptExecutorBase> &luaScriptExecutor);

            void UnregisterLuaScriptExecutor(const std::shared_ptr<LuaScriptExecutorBase> &luaScriptExecutor);

            std::shared_ptr<LuaScriptExecutorBase> GetLuaScriptExecutor(const size_t UId) const;
        };
    }
}
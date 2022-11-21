#include "LuaGlobalHandler.h"

#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{
    namespace Scripts
    {
        LuaGlobalHandler::LuaGlobalHandler()
        {
        }

        void LuaGlobalHandler::RegisterLuaScriptExecutor(const std::shared_ptr<LuaScriptExecutorBase> &luaScriptExecutor)
        {
            assert(!mRegisteredLuaScriptExecutors.count(luaScriptExecutor->GetUId()));
            mRegisteredLuaScriptExecutors[luaScriptExecutor->GetUId()] = luaScriptExecutor;
        }

        void LuaGlobalHandler::UnregisterLuaScriptExecutor(const std::shared_ptr<LuaScriptExecutorBase> &luaScriptExecutor)
        {
            assert(mRegisteredLuaScriptExecutors.count(luaScriptExecutor->GetUId()));
            mRegisteredLuaScriptExecutors.erase(luaScriptExecutor->GetUId());
        }

        std::shared_ptr<LuaScriptExecutorBase> LuaGlobalHandler::GetLuaScriptExecutor(const size_t UId) const
        {
            assert(mRegisteredLuaScriptExecutors.count(UId));
            return mRegisteredLuaScriptExecutors.at(UId);
        }
    }
}
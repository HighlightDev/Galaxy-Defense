#pragma once

#include "Core/InterThreadCommunicationMgr.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/EngineInputLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"

#include <memory>
#include <vector>

using namespace EngineCore;
using namespace Thread;

namespace EngineCore
{
    class InputComponent;
}

namespace EngineCore
{
    namespace Scripts
    {
        class LuaScriptExecutorBase;

        class LuaScriptProcessor
            : public std::enable_shared_from_this<LuaScriptProcessor>
        {
            InterThreadCommunicationMgr &m_interThreadMgr;

            std::vector<std::shared_ptr<LuaScriptExecutorBase>> mLuaScriptExecutors;

            std::vector<std::shared_ptr<LuaProxy>> mLuaProxies;

            // input events receiver for lua
            std::shared_ptr<EngineInputLuaProxy> mInputLuaProxy;

        public:
            explicit LuaScriptProcessor(InterThreadCommunicationMgr &interThreadMgr);

            void Initialize();

            void Tick(const float deltaTime);

            void RegisterLuaScriptExecutor(const std::shared_ptr<LuaScriptExecutorBase> &luaExecutor);

            std::shared_ptr<EngineInputLuaProxy> GetEngineInputLuaProxy() const;

            std::shared_ptr<LuaProxy> GetLuaProxy(const size_t luaProxyId) const;

            void AddLuaProxy(const std::shared_ptr<LuaProxy> &luaProxy);
        };
    }
}
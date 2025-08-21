#pragma once

#include "Core/GameCore/ScriptingCore/LuaProxies/EngineInputLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/OverlayManagerLuaProxy.h"
#include "Core/InterThreadCommunicationMgr.h"

#include <memory>
#include <vector>

using namespace EngineCore;
using namespace Thread;

namespace EngineCore {
namespace Scripts {
class LuaScriptExecutorBase;

/**
 * @class LuaScriptProcessor
 * @brief Manages Lua script execution and communication within the game engine.
 *
 * The LuaScriptProcessor class is responsible for handling the lifecycle and execution
 * of Lua scripts, managing Lua proxies, and facilitating communication between Lua scripts
 * and other engine subsystems such as input and overlay management. It provides methods
 * for registering and unregistering Lua script executors, accessing Lua proxies, and
 * interacting with inter-thread communication mechanisms.
 *
 * @note This class is designed to be used with shared pointers and supports shared_from_this.
 *
 * @see LuaScriptExecutorBase
 * @see LuaProxy
 * @see EngineInputLuaProxy
 * @see OverlayManagerLuaProxy
 * @see InterThreadCommunicationMgr
 */
class LuaScriptProcessor : public std::enable_shared_from_this<LuaScriptProcessor> {
    InterThreadCommunicationMgr& m_interThreadMgr;

    std::vector<std::shared_ptr<LuaScriptExecutorBase>> mLuaScriptExecutors;

    std::vector<std::shared_ptr<LuaProxy>> mLuaProxies;

    // input events receiver for lua
    std::shared_ptr<EngineInputLuaProxy> mInputLuaProxy;

    // overlay manager
    std::shared_ptr<OverlayManagerLuaProxy> mOverlayManagerLuaProxy;

public:
    explicit LuaScriptProcessor(InterThreadCommunicationMgr& interThreadMgr);

    void CleanUp();

    void Tick(const float deltaTime);

    void RegisterLuaScriptExecutor(const std::shared_ptr<LuaScriptExecutorBase>& luaExecutor);

    void UnregisterLuaScriptExecutor(const size_t uid);

    std::shared_ptr<LuaScriptExecutorBase> GetLuaScriptExecutor(const size_t uid) const;

    std::shared_ptr<EngineInputLuaProxy> GetEngineInputLuaProxy() const;

    std::shared_ptr<OverlayManagerLuaProxy> GetOverlayManagerLuaProxy() const;

    void SetOverlayManagerLuaProxy(const std::shared_ptr<OverlayManagerLuaProxy>& overlayManagerLuaProxy);

    std::shared_ptr<LuaProxy> GetLuaProxy(const size_t luaProxyId) const;

    void AddLuaProxy(const std::shared_ptr<LuaProxy>& luaProxy);

    void RemoveLuaProxy(const int32_t luaProxyId);

    InterThreadCommunicationMgr& GetInterThreadCommunicationManager();

private:
    void Initialize();
};
} // namespace Scripts
} // namespace EngineCore
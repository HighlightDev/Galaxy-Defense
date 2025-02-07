#pragma once
#include <stdint.h>

#include <cstddef>
#include <cstdint>
#include <list>
#include <string>
#include <unordered_set>
#include <vector>

extern "C" {
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

namespace EngineCore {
namespace Scripts {
class LuaWrapper {
private:
    lua_State* mState;

    bool mIsLuaScriptOpened{false};

public:
    LuaWrapper();

    ~LuaWrapper();

    lua_State* GetState() const;

    bool ExecuteScript(const std::string& absPath);

    void StopExecution();

    std::string GetErrorMessageAt(int32_t stackIndex) const;

    bool IsLuaScriptOpened() const;

    void ReopenState();
};
} // namespace Scripts
} // namespace EngineCore

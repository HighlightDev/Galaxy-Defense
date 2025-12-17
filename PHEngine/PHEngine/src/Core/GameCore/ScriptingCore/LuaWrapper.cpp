#include "LuaWrapper.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <algorithm>
#include <iostream>

using namespace TinyLogger;
using namespace IO;

namespace EngineCore {
namespace Scripts {

LuaWrapper::LuaWrapper()
{
    mState = luaL_newstate();
    luaL_openlibs(mState);
}

LuaWrapper::~LuaWrapper()
{
    StopExecution();
}

bool LuaWrapper::ExecuteScript(const std::string& absPath)
{
    if (luaL_dofile(mState, absPath.c_str()) != LUA_OK) {
        const auto& message = GetErrorMessageAt(-1);
        LogInfo("ERROR: Lua script execution failed: " + message);
        return false;
    }

    mIsLuaScriptOpened = true;
    return true;
}

void LuaWrapper::StopExecution()
{
    if (mIsLuaScriptOpened && mState) {
        lua_close(mState);
        mState = nullptr;
        mIsLuaScriptOpened = false;
    }
}

void LuaWrapper::ReopenState()
{
    if (!mState) {
        mState = luaL_newstate();
        luaL_openlibs(mState);
    }
}

std::string LuaWrapper::GetErrorMessageAt(int32_t stackIndex) const
{
    return lua_tostring(mState, stackIndex);
}

lua_State* LuaWrapper::GetState() const
{
    return mState;
}

bool LuaWrapper::IsLuaScriptOpened() const
{
    return mIsLuaScriptOpened;
}
} // namespace Scripts
} // namespace EngineCore
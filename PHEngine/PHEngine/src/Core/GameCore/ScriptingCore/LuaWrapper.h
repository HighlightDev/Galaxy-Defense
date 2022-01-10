#pragma once
#include <iostream>
#include <string>
#include <cstddef>

extern "C"
{
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

namespace Game
{
   class LuaWrapper
   {
   private:

      lua_State* mState;

   public:

      LuaWrapper();

      ~LuaWrapper();

      lua_State* GetState() const;

      bool ExecuteScript(const std::string& absPath);

      std::string GetErrorMessageAt(int32_t stackIndex) const;
   };
}

#pragma once
#include <iostream>
#include <string>
#include <stdint.h>

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

      bool ExecuteScript(const std::string& pathToFile);

      std::string GetErrorMessageAt(int32_t stackIndex) const;
   };
}

#pragma once
#include <string>
#include <cstddef>
#include <unordered_set>
#include <list>
#include <vector>

extern "C"
{
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

namespace EngineCore
{
   namespace Scripts
   {
      class LuaWrapper
      {
      private:
         lua_State *mState;

         bool mIsLuaScriptOpened{false};

      public:
         LuaWrapper();

         ~LuaWrapper();

         lua_State *GetState() const;

         bool ExecuteScript(const std::string &absPath);

         void StopExecution();

         std::string GetErrorMessageAt(int32_t stackIndex) const;

         bool IsLuaScriptOpened() const;
      };
   }
}

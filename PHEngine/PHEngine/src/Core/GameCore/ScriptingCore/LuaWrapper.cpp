#include "LuaWrapper.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace TinyLogger;

namespace EngineCore
{
   namespace Scripts
   {

      LuaWrapper::LuaWrapper()
      {
         mState = luaL_newstate();
         luaL_openlibs(mState);
      }

      LuaWrapper::~LuaWrapper()
      {
         StopExecution();
         mState = nullptr;
      }

      bool LuaWrapper::ExecuteScript(const std::string &absPath)
      {
         std::string pathToFile = absPath;

         if (luaL_dofile(mState, pathToFile.c_str()) != LUA_OK)
         {
            const auto& message = GetErrorMessageAt(-1);
            LogInfo(message);
            return false;
         }

         mIsLuaScriptOpened = true;
         return true;
      }

      void LuaWrapper::StopExecution()
      {
         if (mIsLuaScriptOpened && mState)
         {
            lua_close(mState);
            mIsLuaScriptOpened = false;
         }
      }

      std::string LuaWrapper::GetErrorMessageAt(int32_t stackIndex) const
      {
         return lua_tostring(mState, stackIndex);
      }

      lua_State *LuaWrapper::GetState() const
      {
         return mState;
      }
   }
}
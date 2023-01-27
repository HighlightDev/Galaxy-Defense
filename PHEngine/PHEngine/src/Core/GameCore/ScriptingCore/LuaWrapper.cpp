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
            LogInfo(GetErrorMessageAt(-1));
            return false;
         }

         return true;
      }

      void LuaWrapper::StopExecution()
      {
         if (mState)
         {
            lua_close(mState);
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
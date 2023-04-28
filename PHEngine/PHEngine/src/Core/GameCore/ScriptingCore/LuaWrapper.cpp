#include "LuaWrapper.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

using namespace TinyLogger;
using namespace IO;

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
         if (luaL_dofile(mState, absPath.c_str()) != LUA_OK)
         {
            const auto &message = GetErrorMessageAt(-1);
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
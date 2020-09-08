#include "LuaWrapper.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

namespace Game
{
   LuaWrapper::LuaWrapper()
   {
      mState = luaL_newstate();
   }

   LuaWrapper::~LuaWrapper()
   {
      if (mState)
      {
         lua_close(mState);
         mState = nullptr;
      }
   }

   bool LuaWrapper::ExecuteScript(const std::string& relativePathToFile)
   {
      std::string pathToFile = EngineUtility::ConvertFromRelativeToAbsolutePath(relativePathToFile);

      if (luaL_dofile(mState, pathToFile.c_str()) != LUA_OK)
      {
         std::cout << GetErrorMessageAt(-1) << std::endl;
         return false;
      }

      return true;
   }

   std::string LuaWrapper::GetErrorMessageAt(int32_t stackIndex) const
   {
      return lua_tostring(mState, stackIndex);
   }

   lua_State* LuaWrapper::GetState() const
   {
      return mState;
   }
}
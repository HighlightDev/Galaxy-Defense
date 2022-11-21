#include "LuaHelper.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace EngineCore
{
   namespace Scripts
   {
      bool HasLuaError(const LuaWrapper &instanceWrapper, int32_t luaCallResult)
      {
         if (LUA_OK != luaCallResult)
         {
            LogInfo(instanceWrapper.GetErrorMessageAt(-1));
            return true;
         }

         return false;
      }
   }
}
#include "LuaUiControllerExecutor.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/CommonCore/StringHash.h"

using namespace EngineMath;
using namespace EngineCore;

namespace Game
{
   LuaUiControllerExecutor::LuaUiControllerExecutor(const std::string &scriptName)
       : LuaCommonEngineFunctions(scriptName)
   {
   }

   LuaUiControllerExecutor::~LuaUiControllerExecutor()
   {
   }

   void LuaUiControllerExecutor::RegisterCallbacks()
   {
      LuaCommonEngineFunctions::RegisterCallbacks();
   }
}
#include "LuaUiControllerExecutor.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/CommonCore/StringHash.h"

using namespace EngineMath;
using namespace EngineCore;

namespace Game
{
   LuaUiControllerExecutor::LuaUiControllerExecutor(const std::string &scriptName)
       : LuaScriptExecutorBase(scriptName)
   {
      mLuaFunctions = {std::make_shared<LuaCommonUiFunctions>(this),
                       std::make_shared<LuaCommonEngineFunctions>(this),
                       std::make_shared<LuaEngineEventsFunctions>(this),
                       std::make_shared<LuaGameEventsFunctions>(this)};

      for (const auto &luaFunction : mLuaFunctions)
      {
         luaFunction->Initialize();
      }
   }

   LuaUiControllerExecutor::~LuaUiControllerExecutor()
   {
   }

   void LuaUiControllerExecutor::RunScript()
   {
      LuaScriptExecutorBase::RunScript();

      for (const auto &luaFunction : mLuaFunctions)
      {
         luaFunction->OnScriptStarted(mLuaInstance);
      }
   }

   void LuaUiControllerExecutor::StopScript()
   {
      LuaScriptExecutorBase::StopScript();

      for (const auto &luaFunction : mLuaFunctions)
      {
         luaFunction->OnScriptStopped(mLuaInstance);
      }
   }

   void LuaUiControllerExecutor::RegisterCallbacks()
   {
      for (const auto &luaFunction : mLuaFunctions)
      {
         luaFunction->SetScene(GetScene());
         luaFunction->SetLuaScriptProcessor(GetLuaScriptProcessor());
         luaFunction->RegisterCallbacks(mLuaInstance);
      }
   }
}
#include "LuaUiControllerExecutor.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/CommonCore/StringHash.h"

using namespace EngineMath;
using namespace EngineCore;

namespace Game
{
   LuaUiControllerExecutor::LuaUiControllerExecutor(const std::string &scriptName)
       : LuaScriptExecutorBase(scriptName),
         mLuaCommonUiCallbacks(std::make_unique<LuaCommonUiCallbacks>(this)),
         mLuaCommonEngineFunctions(std::make_unique<LuaCommonEngineFunctions>(this))
   {
   }

   LuaUiControllerExecutor::~LuaUiControllerExecutor()
   {
   }

   void LuaUiControllerExecutor::RunScript()
   {
      LuaScriptExecutorBase::RunScript();

      mLuaCommonUiCallbacks->OnScriptStarted(mLuaInstance);
      mLuaCommonEngineFunctions->OnScriptStarted(mLuaInstance);
   }

   void LuaUiControllerExecutor::StopScript()
   {
      LuaScriptExecutorBase::StopScript();

      mLuaCommonUiCallbacks->OnScriptStopped(mLuaInstance);
      mLuaCommonEngineFunctions->OnScriptStopped(mLuaInstance);
   }

   void LuaUiControllerExecutor::RegisterCallbacks()
   {
      mLuaCommonUiCallbacks->SetScene(GetScene());
      mLuaCommonUiCallbacks->SetLuaScriptProcessor(GetLuaScriptProcessor());

      mLuaCommonEngineFunctions->SetScene(GetScene());
      mLuaCommonEngineFunctions->SetLuaScriptProcessor(GetLuaScriptProcessor());

      mLuaCommonUiCallbacks->RegisterCallbacks(mLuaInstance);
      mLuaCommonEngineFunctions->RegisterCallbacks(mLuaInstance);
   }
}
#include "LuaUiControllerExecutor.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/CommonCore/StringHash.h"

using namespace EngineMath;
using namespace EngineCore;

namespace Game
{
   LuaUiControllerExecutor::LuaUiControllerExecutor(const std::string &scriptName)
       : LuaScriptExecutorBase(scriptName),
         mLuaCommonUiCallbacks(std::make_unique<LuaCommonUiFunctions>(this)),
         mLuaCommonEngineFunctions(std::make_unique<LuaCommonEngineFunctions>(this)),
         mLuaEngineEventsFunctions(std::make_unique<LuaEngineEventsFunctions>(this)),
         mLuaGameEventsFunctions(std::make_unique<LuaGameEventsFunctions>(this))
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
      mLuaEngineEventsFunctions->OnScriptStarted(mLuaInstance);
      mLuaGameEventsFunctions->OnScriptStarted(mLuaInstance);
   }

   void LuaUiControllerExecutor::StopScript()
   {
      LuaScriptExecutorBase::StopScript();

      mLuaCommonUiCallbacks->OnScriptStopped(mLuaInstance);
      mLuaCommonEngineFunctions->OnScriptStopped(mLuaInstance);
      mLuaEngineEventsFunctions->OnScriptStarted(mLuaInstance);
      mLuaGameEventsFunctions->OnScriptStarted(mLuaInstance);
   }

   void LuaUiControllerExecutor::RegisterCallbacks()
   {
      mLuaCommonUiCallbacks->SetScene(GetScene());
      mLuaCommonEngineFunctions->SetScene(GetScene());
      mLuaEngineEventsFunctions->SetScene(GetScene());
      mLuaGameEventsFunctions->SetScene(GetScene());

      mLuaCommonUiCallbacks->SetLuaScriptProcessor(GetLuaScriptProcessor());
      mLuaCommonEngineFunctions->SetLuaScriptProcessor(GetLuaScriptProcessor());
      mLuaEngineEventsFunctions->SetLuaScriptProcessor(GetLuaScriptProcessor());
      mLuaGameEventsFunctions->SetLuaScriptProcessor(GetLuaScriptProcessor());

      mLuaCommonUiCallbacks->RegisterCallbacks(mLuaInstance);
      mLuaCommonEngineFunctions->RegisterCallbacks(mLuaInstance);
      mLuaEngineEventsFunctions->RegisterCallbacks(mLuaInstance);
      mLuaGameEventsFunctions->RegisterCallbacks(mLuaInstance);
   }
}
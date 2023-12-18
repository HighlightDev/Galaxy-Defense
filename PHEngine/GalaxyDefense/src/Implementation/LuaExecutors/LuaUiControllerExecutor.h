#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonUiFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaEngineEventsFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Implementation/LuaFunctions/LuaGameEventsFunctions.h"

using namespace EngineCore::Scripts;

namespace EngineCore
{
   class InputComponent;
}

namespace Game
{
   class LuaUiControllerExecutor
       : public LuaScriptExecutorBase
   {
      std::vector<std::shared_ptr<ILuaFunctionable>> mLuaFunctions;

   public:
      LuaUiControllerExecutor(const std::string &scriptName);

      ~LuaUiControllerExecutor();

      void RunScript() override;

      void StopScript() override;

      void RegisterCallbacks() override;
   };
}
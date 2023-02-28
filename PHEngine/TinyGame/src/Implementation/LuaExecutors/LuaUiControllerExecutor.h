#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonUiCallbacks.h"
#include "Core/GameCore/ScriptingCore/LuaFunctions/LuaCommonEngineFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"

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
      std::unique_ptr<LuaCommonUiCallbacks> mLuaCommonUiCallbacks;
      std::unique_ptr<LuaCommonEngineFunctions> mLuaCommonEngineFunctions;

   public:
      LuaUiControllerExecutor(const std::string &scriptName);

      ~LuaUiControllerExecutor();

      void RunScript() override;

      void StopScript() override;

      void RegisterCallbacks() override;
   };
}
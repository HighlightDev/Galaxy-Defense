#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaCommonEngineFunctions.h"

using namespace EngineCore::Scripts;

namespace EngineCore
{
   class InputComponent;
}

namespace Game
{
   class LuaUiControllerExecutor
       : public LuaCommonEngineFunctions
   {
   public:
      LuaUiControllerExecutor(const std::string &scriptName);

      ~LuaUiControllerExecutor();

      void RegisterCallbacks();
   };
}
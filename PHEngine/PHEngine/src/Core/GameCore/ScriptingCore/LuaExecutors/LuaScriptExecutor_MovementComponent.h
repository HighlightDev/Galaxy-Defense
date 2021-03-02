#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_EngineBase.h"

namespace Game
{
   class MovementComponent;

   class LuaScriptExecutor_MovementComponent
      : public LuaScriptExecutor_EngineBase
   {

      MovementComponent* mOwnerComponent;

   public:

      LuaScriptExecutor_MovementComponent(MovementComponent* owner, const std::string& scriptName);

      ~LuaScriptExecutor_MovementComponent();

      void RegisterCallbacks();

      virtual void RunScript() override;

      // Add route point
      void ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, float>& data);
   };
}
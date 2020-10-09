#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"
#include "Core/IoCore/FolderManager.h"

namespace Game
{
   class MovementComponent;

   class LuaScriptExecutor_MovementComponent
   {

      LuaWrapper mLuaInstance;

      MovementComponent* mOwnerComponent;

      std::string mScriptName;

   public:

      LuaScriptExecutor_MovementComponent(MovementComponent* owner, const std::string& scriptName);

      ~LuaScriptExecutor_MovementComponent();

      void RegisterCallbacks();

      void RunScript();

      // Add route point
      void ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::quat, glm::vec3>& data);
   };
}
#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaCommonEngineFunctions.h"

namespace EngineCore
{
   class PlatformTraverseComponent;

   class LuaScriptExecutor_PlatformTraverseComponent
      : public LuaCommonEngineFunctions
   {

      PlatformTraverseComponent* mOwnerComponent;

   public:

      LuaScriptExecutor_PlatformTraverseComponent(PlatformTraverseComponent* owner, const std::string& scriptName);

      ~LuaScriptExecutor_PlatformTraverseComponent();

      void RegisterCallbacks();

      virtual void RunScript() override;

      // Add route point
      void ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, float>& data);
   };
}
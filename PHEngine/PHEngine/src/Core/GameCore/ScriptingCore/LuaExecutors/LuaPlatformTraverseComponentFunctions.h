#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaCommonEngineFunctions.h"

namespace EngineCore
{
   class PlatformTraverseComponent;
   namespace Scripts
   {
      class LuaPlatformTraverseComponentFunctions
          : public LuaCommonEngineFunctions
      {

         PlatformTraverseComponent *mOwnerComponent;

      public:
         LuaPlatformTraverseComponentFunctions(::EngineCore::PlatformTraverseComponent *owner, const std::string &scriptName);

         ~LuaPlatformTraverseComponentFunctions();

         void RegisterCallbacks();

         virtual void RunScript() override;

         // Add route point
         void AddRoutePoint(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, float> &data);
      };
   }
}
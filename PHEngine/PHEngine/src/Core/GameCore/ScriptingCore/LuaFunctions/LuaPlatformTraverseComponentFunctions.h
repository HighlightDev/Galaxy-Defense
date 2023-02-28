#pragma once

#include "Core/GameCore/Scene.h"
#include "Core/IoCore/FolderManager.h"
#include "ILuaFunctionable.h"

using namespace EngineCore;

namespace EngineCore
{
   class Scene;
}

namespace EngineCore
{
   class LuaScriptExecutorBase;
   class PlatformTraverseComponent;

   namespace Scripts
   {
      class LuaScriptProcessor;

      class LuaPlatformTraverseComponentFunctions
          : public ILuaFunctionable
      {
      protected:
         LuaScriptExecutorBase *mOwnerPtr;

         PlatformTraverseComponent *mOwnerComponent;

         std::weak_ptr<::EngineCore::Scene> mSceneWp;

         std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessor;

      public:
         LuaPlatformTraverseComponentFunctions(::EngineCore::PlatformTraverseComponent *owner, LuaScriptExecutorBase *ownerPtr);

         void SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp) override;

         void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor) override;

         void OnScriptStarted(const LuaWrapper &luaWrapper) override;

         void OnScriptStopped(const LuaWrapper &luaWrapper) override;

         void RegisterCallbacks(const LuaWrapper &luaWrapper) override;

      private:
         // Add route point
         void AddRoutePoint(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, float> &data);
      };
   }
}
#pragma once
#include <tuple>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <memory>

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

using namespace Game;

namespace Labyrinth
{

   class LuaScriptExecutor_LevelBuilder
   {
      // To make sure that shared ptr on component will live while raw pointers on that components are used only within Lua code
      std::unordered_map<uint64_t, std::shared_ptr<Component>> mActiveComponents;

      std::weak_ptr<Scene> mSceneWP;

      LuaWrapper mLuaInstance;

      std::string mScriptName;

   public:

      LuaScriptExecutor_LevelBuilder(std::weak_ptr<Scene> scene, const std::string& scriptName);

      ~LuaScriptExecutor_LevelBuilder();

      void RegisterCallbacks();

      void RunScript();

      // Common callbacks
      /* -------------------  Create Actor ----------------------------*/
      Actor* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3>& actorData);

      /* -------------------  Attach component to Actor ----------------------------*/
      void ExecuteLuaCallback(const std::tuple<Actor*, Component*>& dataToAttachActorToComponent);

      /* -------------------  Create component ----------------------------*/
      Component* ExecuteLuaCallback(const std::tuple<std::string, ComponentData*>& componentData);

      // Specific callbacks
      /* -------------------  Create dir light component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*>& dirLightComponentData);

      /* -------------------  Create dir light projection shadow info --------------------*/
      ProjectedShadowInfo* ExecuteLuaCallback(const std::tuple<int32_t>& dirLightProjectionData);

      ITexture* ExecuteLuaCallback(const std::tuple<std::string>& getTextureResource);
   };

}


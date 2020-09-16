#pragma once

#include "Core/GameCore/Level.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

#include <tuple>
#include <unordered_map>

using namespace Game;

namespace Labyrinth
{

   class SimpleLevel :
      public Level
   {
      using Base = Level;

      std::unordered_map<uint64_t, std::shared_ptr<Component>> mActiveComponents;

   public:

      SimpleLevel(InterThreadCommunicationMgr& threadMgr);

      virtual ~SimpleLevel();

      virtual void LoadLevel() override;

      virtual void PreConstructorInitialize();

      virtual void PostConstructorInitialize();
      
      Actor* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3>& actorData);
      void ExecuteLuaCallback(const std::tuple<Actor*, Component*>& dataToAttachActorToComponent);
      Component* ExecuteLuaCallback(const std::tuple<std::string, ComponentData*>& componentData);
      ComponentData* ExecuteLuaCallback(const std::tuple<glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3>& dirLightComponentData);

      void TestLua();
   };

}


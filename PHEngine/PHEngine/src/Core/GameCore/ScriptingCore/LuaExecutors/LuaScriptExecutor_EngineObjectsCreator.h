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

namespace Game
{

   class LuaScriptExecutor_EngineObjectsCreator
   {
      // To make sure that shared ptr on component will live while raw pointers on that components are used only within Lua code
      std::unordered_map<uint64_t, std::shared_ptr<Component>> mActiveComponents;

      std::weak_ptr<Scene> mSceneWP;

      LuaWrapper mLuaInstance;

      std::string mScriptName;

   public:

      LuaScriptExecutor_EngineObjectsCreator(std::weak_ptr<Scene> scene, const std::string& scriptName);

      ~LuaScriptExecutor_EngineObjectsCreator();

      void RegisterCallbacks();

      void RunScript();

      // Common callbacks
      /* -------------------  Create Actor ----------------------------*/
      Actor* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3>& actorData);

      /* ------------------- Attach actor to player controller ----------------------------*/
      void ExecuteLuaCallback(const std::tuple<Actor*>& actorData);

      /* -------------------  Attach component to Actor ----------------------------*/
      void ExecuteLuaCallback(const std::tuple<Actor*, Component*>& dataToAttachActorToComponent);

      /* -------------------  Create component ----------------------------*/
      Component* ExecuteLuaCallback(const std::tuple<std::string, ComponentData*>& componentData);

      // Specific callbacks

       /* -------------------  Create mesh component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial*>& meshComponentData);
    
      /* -------------------  Create dir light component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*>& dirLightComponentData);

      /* -------------------  Create input component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string>& inputComponentData);

      /* -------------------  Create movement component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, std::string>& movementComponentData);
      
      /* -------------------  Create physics component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, PhysicsDescriptor*>& phyComponentData);

      /* -------------------  Create dir light projection shadow info --------------------*/
      ProjectedShadowInfo* ExecuteLuaCallback(const std::tuple<int32_t>& dirLightProjectionData);

      /* -------------------  Create material --------------------*/
      IMaterial* ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder>& buildMaterial);

      /* -------------------  Set texture --------------------*/
      void ExecuteLuaCallback(const std::tuple<IMaterial*, /* texture name*/std::string, /*property name*/std::string>& setTextureToMaterial);

      /* -------------------  Set float --------------------*/
      void ExecuteLuaCallback(const std::tuple<IMaterial*, float, std::string>& setFloatValueToMaterial);

      /*-------------------- Create physics collision sphere shape --------------*/
      PhyShapeBase* ExecuteLuaCallback(const std::tuple<float>& value);

      /*-------------------- Create physics collision box shape --------------*/
      PhyShapeBase* ExecuteLuaCallback(const std::tuple<glm::vec3>& halfExtent);

      /*-------------------- Create physics collision capsule shape --------------*/
      PhyShapeBase* ExecuteLuaCallback(const std::tuple<float, float>& capsuleData);

      /*-------------------- Create physics collision plane shape --------------*/
      PhyShapeBase* ExecuteLuaCallback(const std::tuple<glm::vec3, float> planeData);

      /*-------------------- Create rigid body controller--------------*/
      PhysicsDescriptor* ExecuteLuaCallback(const std::tuple<PhyShapeBase*, std::string, float> descData);

      /*-------------------- Create dynamic character controller--------------*/
      PhysicsDescriptor* ExecuteLuaCallback(const std::tuple<float, float, float, float> descData);

      /* -------------------  Create State machine ----------------------------*/
      StateMachine* ExecuteLuaCallback(const std::tuple<Actor*, std::string>& fsmData);
    
      /* -------------------  Set bindings ------------------------*/
      void ExecuteLuaCallback(const std::tuple<StateMachine*, std::string, std::string, std::string>& fsmData);
   };

}


#pragma once
#include <unordered_map>
#include <glm/vec3.hpp>
#include <memory>

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ACamera.h"
#include "LuaScriptExecutor_EngineBase.h"

namespace Game
{

   class LuaScriptExecutor_EngineObjectsCreator
      : public LuaScriptExecutor_EngineBase
   {
      // To make sure that shared ptr on component will live while raw pointers on that components are used only within Lua code
      std::unordered_map<uint64_t, std::shared_ptr<Component>> mActiveComponents;

      // To avoid memory leaks
      std::vector<ComponentData*> mAllocatedComponentData;

   public:

      LuaScriptExecutor_EngineObjectsCreator(const std::string& scriptName);

      ~LuaScriptExecutor_EngineObjectsCreator();

      virtual void RegisterCallbacks() override;

      virtual void RunScript() override;

      // Common callbacks
      /* -------------------  Create Actor ----------------------------*/
      Actor* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3>& actorData);

      /* -------------------  Create third person camera ----------------------------*/
      void ExecuteLuaCallback(const std::tuple<std::string, glm::ivec4, float, float, float, glm::vec3, int32_t>& cameraData);

      /* ------------------- Attach actor to player controller ----------------------------*/
      void ExecuteLuaCallback(const std::tuple<Actor*>& actorData);

      /* -------------------  Attach component to Actor ----------------------------*/
      void ExecuteLuaCallback(const std::tuple<Actor*, Component*>& dataToAttachActorToComponent);

      /* -------------------  Create component ----------------------------*/
      Component* ExecuteLuaCallback(const std::tuple<std::string, ComponentData*>& componentData);

      /* -------------------  Create mesh component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial*>& meshComponentData);

      /* -------------------  Create simple mesh component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<LuaArgDummyPlaceholder<>, std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial*>& meshComponentData);

      /* -------------------  Create water plane component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial*>&  data);

      /* -------------------  Create planar reflection component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, std::string/*Camera name*/, glm::ivec4>&  data);

      /* -------------------  Create dir light component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo*>& dirLightComponentData);

      /* -------------------  Create point light component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3,
         glm::vec3, glm::vec3, glm::vec3, float, ProjectedShadowInfo*>& pointLightComponentData);

      /* -------------------  Create spotlight component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3,
         glm::vec3, glm::vec3, glm::vec3, float, float, ProjectedShadowInfo*>& pointLightComponentData);

      /* -------------------  Create input component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string>& inputComponentData);

      /* -------------------  Create character movement component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, std::string>& movementComponentData);

      /* -------------------  Create movement component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, std::string>& movementComponentData);

      /* -------------------  Create physics component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, PhysicsDescriptor*>& phyComponentData);

      /* -------------------  Create skybox component data ----------------------------*/
      ComponentData* ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, IMaterial*>& skyboxComponentData);

      /* -------------------  Create light projection shadow info --------------------*/
      ProjectedShadowInfo* ExecuteLuaCallback(const std::tuple<int32_t, std::string>& dirLightProjectionData);

      /* -------------------  Create material --------------------*/
      IMaterial* ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder<>>& buildMaterial);

      /* -------------------  Set texture --------------------*/
      void ExecuteLuaCallback(const std::tuple<IMaterial*, /* texture name*/std::string, /*property name*/std::string>& setTextureToMaterial);

      /* -------------------  Set float --------------------*/
      void ExecuteLuaCallback(const std::tuple<IMaterial*, float, std::string>& setFloatValueToMaterial);

      /* -------------------  Set deferred texture --------------------*/
      void ExecuteLuaCallback(const std::tuple<LuaArgDummyPlaceholder<>, IMaterial*, /*deferred resource creator name*/std::string, /*property name*/std::string>& setDeferredTextureToMaterial);

      /* -------------------  Set binding to material --------------------*/
      void ExecuteLuaCallback(const std::tuple<IMaterial*, /*game object name*/std::string, /*gamePropertyName*/std::string, /*bindingName*/std::string>& setBindingToMaterial);

      /*-------------------- Create physics collision sphere shape --------------*/
      PhysicsShapeBase* ExecuteLuaCallback(const std::tuple<double>& value);

      /*-------------------- Create physics collision box shape --------------*/
      PhysicsShapeBase* ExecuteLuaCallback(const std::tuple<glm::vec3>& halfExtent);

      /*-------------------- Create physics collision capsule shape --------------*/
      PhysicsShapeBase* ExecuteLuaCallback(const std::tuple<double, double>& capsuleData);

      /*-------------------- Create physics collision plane shape --------------*/
      PhysicsShapeBase* ExecuteLuaCallback(const std::tuple<glm::vec3, double> planeData);

      /*-------------------- Create physics compound shape --------------*/
      PhysicsShapeBase* ExecuteLuaCallback(const std::tuple<>& noData);

      /*-------------------- Add child shape to compound --------------*/
      void ExecuteLuaCallback(const std::tuple</*compound shape*/PhysicsShapeBase*,
         /*child shape*/ PhysicsShapeBase*, glm::vec3/*child translation*/, glm::vec3/*child rotation*/>& data);

      /*-------------------- Create rigid body controller--------------*/
      PhysicsDescriptor* ExecuteLuaCallback(const std::tuple<PhysicsShapeBase*, std::string, float> descData);

      /*-------------------- Create dynamic character controller--------------*/
      PhysicsDescriptor* ExecuteLuaCallback(const std::tuple<float, float, float, float> descData);

      /* -------------------  Create Tweener ----------------------------*/
      Tweener* ExecuteLuaCallback(const std::tuple<Actor*, std::string>& tweenerData);

      /* -------------------  Set tweener bindings ------------------------*/
      void ExecuteLuaCallback(const std::tuple<Tweener*, std::string, std::string, std::string>& tweenerData);
   };

}


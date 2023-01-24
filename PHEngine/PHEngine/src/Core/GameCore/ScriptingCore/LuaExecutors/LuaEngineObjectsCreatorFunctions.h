#pragma once
#include <unordered_map>
#include <glm/vec3.hpp>
#include <memory>

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ACamera.h"
#include "LuaCommonEngineFunctions.h"

using namespace EngineCore;

namespace EngineCore
{
   namespace Scripts
   {
      class LuaEngineObjectsCreatorFunctions
          : public LuaCommonEngineFunctions
      {
         // To make sure that shared ptr on component will live while raw pointers on that components are used only within Lua code
         std::unordered_map<uint64_t, std::shared_ptr<Component>> mActiveComponents;

         // To avoid memory leaks
         std::vector<ComponentData *> mAllocatedComponentData;

      public:
         LuaEngineObjectsCreatorFunctions(const std::string &scriptName);

         ~LuaEngineObjectsCreatorFunctions();

         void RegisterCallbacks() override;

         void RunScript() override;

         // Common callbacks
         /* -------------------  Load asynchronously resources by names ----------------------------*/
         void LoadResourcesAsync(const std::tuple<std::string> &asyncLoadNamesData);

         void LazyLoadResourcesAsync(const std::tuple<std::string> &dataNames);

         /* -------------------  Create Actor ----------------------------*/
         Actor *CreateActor(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3> &actorData);

         /* -------------------  Create third person camera ----------------------------*/
         void CreateThirdPersonCamera(const std::tuple<std::string, glm::ivec4, float, float, float, glm::vec3, int32_t> &cameraData);

         /* -------------------  Create first person camera ----------------------------*/
         void CreateFirstPersonCamera(const std::tuple<std::string, glm::ivec4, float, float, glm::vec3, int32_t> &cameraData);

         /* ------------------- Attach actor to player controller ----------------------------*/
         void AttachPlayerControllerToActor(const std::tuple<Actor *> &actorData);

         /* -------------------  Attach component to Actor ----------------------------*/
         void AttachComponentToActor(const std::tuple<std::string, Component *> &dataToAttachActorToComponent);

         /* -------------------  Create component ----------------------------*/
         Component *CreateComponent(const std::tuple<std::string, ComponentData *> &componentData);

         /* -------------------  Create mesh component data ----------------------------*/
         ComponentData *CreateMeshComponentData(const std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *> &meshComponentData);

         /* -------------------  Create water plane component data ----------------------------*/
         ComponentData *CreateWaterPlaneComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial *> &data);

         /* -------------------  Create planar reflection component data ----------------------------*/
         ComponentData *CreatePlanarReflectionComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, std::string /*Camera name*/, glm::ivec4> &data);

         /* -------------------  Create dir light component data ----------------------------*/
         ComponentData *CreateDirLightComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo *> &dirLightComponentData);

         /* -------------------  Create point light component data ----------------------------*/
         ComponentData *CreatePointLightComponentData(const std::tuple<std::string, glm::vec3, glm::vec3,
                                                                       glm::vec3, glm::vec3, glm::vec3, float, ProjectedShadowInfo *> &pointLightComponentData);

         /* -------------------  Create spotlight component data ----------------------------*/
         ComponentData *CreateSpotlightComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3,
                                                                      glm::vec3, glm::vec3, glm::vec3, float, float, ProjectedShadowInfo *> &pointLightComponentData);

         /* -------------------  Create input component data ----------------------------*/
         ComponentData *CreateInputComponentData(const std::tuple<std::string> &inputComponentData);

         /* -------------------  Create character movement component data ----------------------------*/
         ComponentData *CreateCharacterMovementComponentData(const std::tuple<std::string, glm::vec3, std::string> &movementComponentData);

         /* -------------------  Create movement component data ----------------------------*/
         ComponentData *CreatePlatformTraverseComponentData(const std::tuple<std::string, std::string> &movementComponentData);

         /* -------------------  Create physics component data ----------------------------*/
         ComponentData *CreatePhysicsComponentData(const std::tuple<std::string, PhysicsDescriptor *> &phyComponentData);

         /* -------------------  Create skybox component data ----------------------------*/
         ComponentData *CreateSkyboxComponentData(const std::tuple<std::string, glm::vec3, IMaterial *> &skyboxComponentData);

         /* -------------------  Create light projection shadow info --------------------*/
         ProjectedShadowInfo *CreateLightProjectionShadowInfo(const std::tuple<int32_t, std::string> &dirLightProjectionData);

         /* -------------------  Create material --------------------*/
         IMaterial *CreateMaterial(const std::tuple<std::string> &buildMaterial);

         /* -------------------  Set texture --------------------*/
         void SetTextureToMaterial(const std::tuple<IMaterial *, /* texture name*/ std::string, /*property name*/ std::string> &setTextureToMaterial);

         /* -------------------  Set float --------------------*/
         void SetFloatToMaterial(const std::tuple<IMaterial *, float, std::string> &setFloatValueToMaterial);

         /* -------------------  Set deferred texture --------------------*/
         void SetDeferredTextureToMaterial(const std::tuple<IMaterial *, /*deferred resource creator name*/ std::string, /*property name*/ std::string> &setDeferredTextureToMaterial);

         /* -------------------  Set binding to material --------------------*/
         void SetBindingToMaterial(const std::tuple<IMaterial *, /*game object name*/ std::string, /*gamePropertyName*/ std::string, /*bindingName*/ std::string> &setBindingToMaterial);

         /*-------------------- Create physics collision sphere shape --------------*/
         PhysicsShapeBase *CreatePhysicsSphereShape(const std::tuple<double> &value);

         /*-------------------- Create physics collision box shape --------------*/
         PhysicsShapeBase *CreatePhysicsBoxShape(const std::tuple<glm::vec3> &halfExtent);

         /*-------------------- Create physics collision capsule shape --------------*/
         PhysicsShapeBase *CreatePhysicsCapsuleShape(const std::tuple<double, double> &capsuleData);

         /*-------------------- Create physics collision plane shape --------------*/
         PhysicsShapeBase *CreatePhysicsPlaneShape(const std::tuple<glm::vec3, double> planeData);

         /*-------------------- Create physics compound shape --------------*/
         PhysicsShapeBase *CreatePhysicsCompoundShape(const std::tuple<> &noData);

         /*-------------------- Add child shape to compound --------------*/
         void AddCompoundChildShape(const std::tuple</*compound shape*/ PhysicsShapeBase *,
                                                     /*child shape*/ PhysicsShapeBase *, glm::vec3 /*child translation*/, glm::vec3 /*child rotation*/> &data);

         /*-------------------- Create rigid body controller--------------*/
         PhysicsDescriptor *CreateRigidBodyController(const std::tuple<PhysicsShapeBase *, std::string, float> descData);

         /*-------------------- Create dynamic character controller--------------*/
         PhysicsDescriptor *CreateDynamicCharacterController(const std::tuple<float, float, float, float> descData);

         /* -------------------  Create Tweener ----------------------------*/
         Tweener *CreateTweener(const std::tuple<Actor *, std::string> &tweenerData);

         /* -------------------  Set tweener bindings ------------------------*/
         void SetTweenerBinding(const std::tuple<Tweener *, std::string, std::string, std::string> &tweenerData);
      };
   }
}

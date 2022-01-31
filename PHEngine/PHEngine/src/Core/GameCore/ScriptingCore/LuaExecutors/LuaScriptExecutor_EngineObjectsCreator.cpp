#include "LuaScriptExecutor_EngineObjectsCreator.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"

using namespace Graphics;

namespace Game
{

   LuaScriptExecutor_EngineObjectsCreator::LuaScriptExecutor_EngineObjectsCreator(const std::string &scriptName)
       : LuaScriptExecutor_EngineBase(scriptName), mActiveComponents(), mAllocatedComponentData()
   {
   }

   LuaScriptExecutor_EngineObjectsCreator::~LuaScriptExecutor_EngineObjectsCreator()
   {
      for (auto dataPtr : mAllocatedComponentData)
      {
         delete dataPtr;
      }
   }

   void LuaScriptExecutor_EngineObjectsCreator::RegisterCallbacks()
   {
      LuaScriptExecutor_EngineBase::RegisterCallbacks();

      using LuaExecutor_t = LuaScriptExecutor_EngineObjectsCreator;

      LuaRegisterCallback<LuaExecutor_t, void(LuaArgDummyPlaceholder<>, std::string)>::Register(mLuaInstance, "_LoadResourcesAsync");

      LuaRegisterCallback<LuaExecutor_t, Component *(std::string, ComponentData *)>::Register(mLuaInstance, "_CreateComponent");
      LuaRegisterCallback<LuaExecutor_t, Actor *(std::string, glm::vec3, glm::vec3, glm::vec3)>::Register(mLuaInstance, "_CreateActor");

      LuaRegisterCallback<LuaExecutor_t, void(std::string, glm::ivec4, float, float, float, glm::vec3, int32_t)>::Register(mLuaInstance, "_CreateThirdPersonCamera");
      LuaRegisterCallback<LuaExecutor_t, void(std::string, glm::ivec4, float, float, glm::vec3)>::Register(mLuaInstance, "_CreateFirstPersonCamera");

      LuaRegisterCallback<LuaExecutor_t, void(Actor *, Component *)>::Register(mLuaInstance, "_AttachComponentToActor");
      LuaRegisterCallback<LuaExecutor_t, void(Actor *)>::Register(mLuaInstance, "_AttachPlayerControllerToActor");

      LuaRegisterCallback<LuaExecutor_t, ProjectedShadowInfo *(int32_t, std::string)>::Register(mLuaInstance, "_CreateLightProjectionShadowInfo");

      /*************************************COMPONENT DATA**************************************/
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo *)>::Register(mLuaInstance, "_CreateDirLightComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, ProjectedShadowInfo *)>::Register(mLuaInstance, "_CreatePointLightComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, float, ProjectedShadowInfo *)>::Register(mLuaInstance, "_CreateSpotlightComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *)>::Register(mLuaInstance, "_CreateMeshComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(LuaArgDummyPlaceholder<>, std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *)>::Register(mLuaInstance, "_CreateSimpleMeshComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, PhysicsDescriptor *)>::Register(mLuaInstance, "_CreatePhysicsComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string)>::Register(mLuaInstance, "_CreateInputComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, glm::vec3, std::string)>::Register(mLuaInstance, "_CreateCharacterMovementComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, std::string)>::Register(mLuaInstance, "_CreatePlatformMovementComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, glm::vec3, IMaterial *)>::Register(mLuaInstance, "_CreateSkyboxComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial *)>::Register(mLuaInstance, "_CreateWaterPlaneComponentData");
      LuaRegisterCallback<LuaExecutor_t, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, std::string /*Camera name*/, glm::ivec4)>::Register(mLuaInstance, "_CreatePlanarReflectionComponentData");
      /*************************************COMPONENT DATA**************************************/

      LuaRegisterCallback<LuaExecutor_t, IMaterial *(std::string, LuaArgDummyPlaceholder<>)>::Register(mLuaInstance, "_CreateMaterial");
      LuaRegisterCallback<LuaExecutor_t, void(IMaterial *, std::string, std::string)>::Register(mLuaInstance, "_SetTextureToMaterial");
      LuaRegisterCallback<LuaExecutor_t, void(IMaterial *, float, std::string)>::Register(mLuaInstance, "_SetFloatToMaterial");
      LuaRegisterCallback<LuaExecutor_t, void(LuaArgDummyPlaceholder<>, IMaterial *, std::string, std::string)>::Register(mLuaInstance, "_SetDeferredTextureToMaterial");
      LuaRegisterCallback<LuaExecutor_t, void(IMaterial *, std::string, std::string, std::string)>::Register(mLuaInstance, "_SetBindingToMaterial");

      /**********************************************PHYSICS COLLISION SHAPE***********************************************/
      LuaRegisterCallback<LuaExecutor_t, PhysicsShapeBase *(glm::vec3)>::Register(mLuaInstance, "_CreatePhysicsBoxShape");
      LuaRegisterCallback<LuaExecutor_t, PhysicsShapeBase *(double)>::Register(mLuaInstance, "_CreatePhysicsSphereShape");
      LuaRegisterCallback<LuaExecutor_t, PhysicsShapeBase *(double, double)>::Register(mLuaInstance, "_CreatePhysicsCapsuleShape");
      LuaRegisterCallback<LuaExecutor_t, PhysicsShapeBase *(glm::vec3, double)>::Register(mLuaInstance, "_CreatePhysicsPlaneShape");
      LuaRegisterCallback<LuaExecutor_t, PhysicsShapeBase *()>::Register(mLuaInstance, "_CreatePhysicsCompoundShape");
      LuaRegisterCallback<LuaExecutor_t, void(PhysicsShapeBase *, PhysicsShapeBase *, glm::vec3, glm::vec3)>::Register(mLuaInstance, "_AddCompoundChildShape");
      /**********************************************PHYSICS COLLISION SHAPE***********************************************/

      LuaRegisterCallback<LuaExecutor_t, PhysicsDescriptor *(PhysicsShapeBase *, std::string, float)>::Register(mLuaInstance, "_CreateRigidBodyController");
      LuaRegisterCallback<LuaExecutor_t, PhysicsDescriptor *(float, float, float, float)>::Register(mLuaInstance, "_CreateDynamicCharacterController");

      LuaRegisterCallback<LuaExecutor_t, Tweener *(Actor *, std::string)>::Register(mLuaInstance, "_CreateTweener");
      LuaRegisterCallback<LuaExecutor_t, void(Tweener *, std::string, std::string, std::string)>::Register(mLuaInstance, "_SetTweenerBinding");
   }

   void LuaScriptExecutor_EngineObjectsCreator::RunScript()
   {
      LuaScriptExecutor_EngineBase::RunScript();
   }

   /* -------------------  Load asynchronously resources by names ----------------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<LuaArgDummyPlaceholder<>, std::string> &asyncLoadNamesData)
   {
      const std::string &resourcesNamesStr = std::get<1>(asyncLoadNamesData);
      assert(!resourcesNamesStr.empty());

      const std::vector<std::string> &resourceNames = Split(resourcesNamesStr, ',');

      for (std::string resName : resourceNames)
      {
         resName = TrimEnd(resName);
         ResourceMap::GetInstance()->AllocateAsync(resName);
      }

      ResourceMap::GetInstance()->WaitUntilResourcesLoad();
   }

   /* -------------------  Create Actor ----------------------------*/
   Actor *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3> &actorData)
   {
      Actor *createdActor = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         auto actorSP = EngineObjectCreator::CreateActorByString(std::get<0>(actorData), std::make_shared<Game::SceneComponent>(std::get<0>(actorData) + "rootComponent", std::get<1>(actorData), std::get<2>(actorData), std::get<3>(actorData)));
         scene->AddActor(actorSP);
         createdActor = actorSP.get();
      }

      return createdActor;
   }

   /* -------------------  Attach component to Actor ----------------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<Actor *, Component *> &dataToAttachActorToComponent)
   {
      Actor *actor = std::get<0>(dataToAttachActorToComponent);
      Component *component = std::get<1>(dataToAttachActorToComponent);

      assert((actor && component, "Actor or component is null"));

      const bool componentExists = mActiveComponents.count(component->GetObjectId());
      assert((componentExists, "Sought component doesn't exist"));

      actor->AddComponent(mActiveComponents[component->GetObjectId()]);
   }

   /* -------------------  Create first person camera ----------------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string /*cameraName*/,
                                                                                    glm::ivec4 /*viewPort*/,
                                                                                    float /*initPitchDeg*/,
                                                                                    float /*initYawDeg*/,
                                                                                    glm::vec3 /*init camera position*/> &cameraData)
   {
      if (auto scene = mSceneWP.lock())
      {
         const glm::ivec4 &viewPortData = std::get<1>(cameraData);

         const auto &firstPersonCamera = EngineObjectCreator::CreateFirstPersonCamera(
             std::get<0>(cameraData),
             scene,
             ViewPortInfo(viewPortData.x, viewPortData.y, viewPortData.z, viewPortData.w),
             std::get<2>(cameraData),
             std::get<3>(cameraData),
             std::get<4>(cameraData));

         scene->RegisterCamera(firstPersonCamera);
      }
   }

   /* -------------------  Create third person camera ----------------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string /*cameraName*/,
                                                                                    glm::ivec4 /*viewPort*/, float /*initPitchDeg*/, float /*initYawDeg*/, float /*camDistanceToThirdPersonTarget*/, glm::vec3 /*thirdPersonTargetOffset*/,
                                                                                    int32_t /*is main camera in the scene*/> &cameraData)
   {
      if (auto scene = mSceneWP.lock())
      {
         const glm::ivec4 &viewPortData = std::get<1>(cameraData);
         const bool bIsMainSceneCamera = static_cast<int32_t>(std::get<6>(cameraData));

         auto thirdPersonCamera = EngineObjectCreator::CreateThirdPersonCamera(
             std::get<0>(cameraData),
             scene,
             ViewPortInfo(viewPortData.x, viewPortData.y, viewPortData.z, viewPortData.w),
             std::get<2>(cameraData),
             std::get<3>(cameraData),
             std::get<4>(cameraData),
             std::get<5>(cameraData),
             bIsMainSceneCamera);

         if (bIsMainSceneCamera)
         {
            scene->RegisterMainCamera(thirdPersonCamera);
         }
         else
         {
            scene->RegisterCamera(thirdPersonCamera);
         }
      }
   }

   /* ------------------- Attach actor to player controller ----------------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<Actor *> &actorData)
   {
      Actor *actor = std::get<0>(actorData);
      assert((actor, "Actor is null"));

      if (auto scene = mSceneWP.lock())
      {
         auto camera = scene->GetMainCamera();
         assert(camera && eCameraType::MAIN_THIRD_PERSON_CAMERA == camera->GetCameraType());

         auto actorIt = std::find_if(scene->GetActors().begin(), scene->GetActors().end(),
                                     [&](const std::shared_ptr<Actor> &sceneActor)
                                     { return sceneActor->GetObjectId() == actor->GetObjectId(); });
         assert(actorIt != scene->GetActors().end());

         scene->SetPlayerController(std::make_shared<PlayerController>(camera, (*actorIt)));
         std::static_pointer_cast<ThirdPersonCamera>(camera)->SetThirdPersonTargetDeferred((*actorIt)->GetGameObjectName());
      }
   }

   /* -------------------  Create component ----------------------------*/
   Component *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, ComponentData *> &componentData)
   {
      Component *createdComponent = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         std::shared_ptr<Component> component = EngineObjectCreator::CreateComponentByString(std::get<0>(componentData), std::get<1>(componentData), scene);
         assert(component);

         mActiveComponents[component->GetObjectId()] = component;
         createdComponent = component.get();
      }

      return createdComponent;
   }

   /* -------------------  Create mesh component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *> &meshComponentData)
   {
      const auto &meshModelName = std::get<1>(meshComponentData);
      auto dataPtr = EngineObjectCreator::CreateMeshComponentData(std::get<0>(meshComponentData),
                                                                  meshModelName, std::get<2>(meshComponentData),
                                                                  std::get<3>(meshComponentData), std::get<4>(meshComponentData), std::get<5>(meshComponentData), std::get<6>(meshComponentData));

      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create simple mesh component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<LuaArgDummyPlaceholder<>, std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *> &meshComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreateSimpleMeshComponentData(std::get<1>(meshComponentData), std::get<2>(meshComponentData), std::get<3>(meshComponentData),
                                                                        std::get<4>(meshComponentData), std::get<5>(meshComponentData), std::get<6>(meshComponentData), std::get<7>(meshComponentData));

      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create water plane component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial *> &data)
   {
      auto dataPtr = EngineObjectCreator::CreateWaterPlaneComponentData(std::get<0>(data), std::get<1>(data), std::get<2>(data),
                                                                        std::get<3>(data), std::get<4>(data));

      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create planar reflection component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, std::string /*Camera name*/, glm::ivec4> &data)
   {
      ComponentData *dataPtr = nullptr;
      if (auto scene = mSceneWP.lock())
      {
         auto camera = scene->GetCamera(std::get<4>(data));
         const glm::ivec4 &viewPortInfo = std::get<5>(data);

         dataPtr = EngineObjectCreator::CreatePlanarReflectionComponentData(std::get<0>(data), std::get<1>(data), std::get<2>(data),
                                                                            std::get<3>(data), camera.get(), ViewPortInfo(viewPortInfo.x, viewPortInfo.y, viewPortInfo.z, viewPortInfo.w));

         mAllocatedComponentData.push_back(dataPtr);
      }
      return dataPtr;
   }

   /* -------------------  Create dir light component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo *> &dirLightComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreateDirLightComponentData(std::get<0>(dirLightComponentData), std::get<1>(dirLightComponentData), std::get<2>(dirLightComponentData),
                                                                      std::get<3>(dirLightComponentData), std::get<4>(dirLightComponentData), std::get<5>(dirLightComponentData), std::get<6>(dirLightComponentData));

      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create point light component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, ProjectedShadowInfo *> &pointLightComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreatePointLightComponentData(
          std::get<0>(pointLightComponentData), std::get<1>(pointLightComponentData),
          std::get<2>(pointLightComponentData), std::get<3>(pointLightComponentData),
          std::get<4>(pointLightComponentData), std::get<5>(pointLightComponentData),
          std::get<6>(pointLightComponentData), std::get<7>(pointLightComponentData));

      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create spotlight component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3,
                                                                                              glm::vec3, glm::vec3, glm::vec3, float, float, ProjectedShadowInfo *> &spotlightComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreateSpotlightComponentData(
          std::get<0>(spotlightComponentData), std::get<1>(spotlightComponentData),
          std::get<2>(spotlightComponentData), std::get<3>(spotlightComponentData),
          std::get<4>(spotlightComponentData), std::get<5>(spotlightComponentData),
          std::get<6>(spotlightComponentData), std::get<7>(spotlightComponentData),
          std::get<8>(spotlightComponentData), std::get<9>(spotlightComponentData));

      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create physics component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, PhysicsDescriptor *> &phyComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreatePhysicsComponentData(std::get<0>(phyComponentData), std::get<1>(phyComponentData));
      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create input component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string> &inputComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreateInputComponentData(std::get<0>(inputComponentData));
      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create character movement component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, std::string> &movementComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreateCharacterMovementComponentData(std::get<0>(movementComponentData), std::get<1>(movementComponentData), std::get<2>(movementComponentData));
      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create platform movement component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, std::string> &platformMovementComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreatePlatformMovementComponentData(std::get<0>(platformMovementComponentData), std::get<1>(platformMovementComponentData));
      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create skybox component data ----------------------------*/
   ComponentData *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, IMaterial *> &skyboxComponentData)
   {
      auto dataPtr = EngineObjectCreator::CreateSkyboxComponentData(std::get<0>(skyboxComponentData), std::get<1>(skyboxComponentData), std::get<2>(skyboxComponentData));
      mAllocatedComponentData.push_back(dataPtr);
      return dataPtr;
   }

   /* -------------------  Create light projection shadow info --------------------*/
   ProjectedShadowInfo *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<int32_t, std::string> &lightProjectionData)
   {
      ProjectedShadowInfo *shadowProjInfo = nullptr;

      const int32_t shadowAtlasSize = std::get<0>(lightProjectionData);
      const std::string &lightType = std::get<1>(lightProjectionData);

      shadowProjInfo = EngineObjectCreator::CreateProjectedShadowInfo(lightType, glm::ivec2(shadowAtlasSize, shadowAtlasSize));

      return shadowProjInfo;
   }

   /* -------------------  Create material --------------------*/
   IMaterial *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<std::string, LuaArgDummyPlaceholder<>> &buildMaterial)
   {
      MaterialParser materialParser;
      const std::string &materialName = std::get<0>(buildMaterial);
      return materialParser.ParseMaterialDescriptor(materialName);
   }

   /* -------------------  Set texture --------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<IMaterial *, std::string, std::string> &setTextureToMaterial)
   {
      IMaterial *material = std::get<0>(setTextureToMaterial);
      const std::string &textureNames = std::get<1>(setTextureToMaterial);
      const std::string &propertyName = std::get<2>(setTextureToMaterial);
      const auto &texture = TexturePool::GetInstance()->GetOrAllocateResource(textureNames);
      MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, texture);
   }

   /* -------------------  Set deferred texture --------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<LuaArgDummyPlaceholder<>, IMaterial *, /*deferred resource creator name*/ std::string, /*property name*/ std::string> &data)
   {
      IMaterial *material = std::get<1>(data);
      const std::string &resourceCreatorName = std::get<2>(data);
      const std::string &propertyName = std::get<3>(data);

      if (auto scene = mSceneWP.lock())
      {
         IDeferredResourceCreator *resourceCreator = scene->GetDeferredResourceCreatorByName(resourceCreatorName);
         assert(resourceCreator);
         MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, resourceCreator);
      }
   }

   /* -------------------  Set float --------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<IMaterial *, float, std::string> &setFloatValueToMaterial)
   {
      IMaterial *material = std::get<0>(setFloatValueToMaterial);
      float value = std::get<1>(setFloatValueToMaterial);
      const std::string &propertyName = std::get<2>(setFloatValueToMaterial);
      MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, value);
   }

   /* -------------------  Set binding to material --------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<IMaterial *, std::string, std::string, std::string> &setBindingToMaterial)
   {
      if (auto scene = mSceneWP.lock())
      {
         IMaterial *material = std::get<0>(setBindingToMaterial);
         const std::string &gameObjectName = std::get<1>(setBindingToMaterial);
         const std::string &gamePropertyName = std::get<2>(setBindingToMaterial);
         const std::string &bindingName = std::get<3>(setBindingToMaterial);

         const GameObject *gameObject = scene->GetGameObjectByName(gameObjectName);
         MaterialPropertySetter::SetMaterialPropertyValue(material, gameObject, gamePropertyName, bindingName);
      }
   }

   /*-------------------- Create physics collision sphere shape --------------*/
   PhysicsShapeBase *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<double> &value)
   {
      return EngineObjectCreator::CreatePhysicsSphereShape(std::get<0>(value));
   }

   /*-------------------- Create physics collision box shape --------------*/
   PhysicsShapeBase *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<glm::vec3> &halfExtent)
   {
      return EngineObjectCreator::CreatePhysicsBoxShape(std::get<0>(halfExtent));
   }

   /*-------------------- Create physics collision capsule shape --------------*/
   PhysicsShapeBase *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<double, double> &capsuleData)
   {
      return EngineObjectCreator::CreatePhysicsCapsuleShape(std::get<0>(capsuleData), std::get<1>(capsuleData));
   }

   /*-------------------- Create physics collision plane shape --------------*/
   PhysicsShapeBase *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<glm::vec3, double> planeData)
   {
      return EngineObjectCreator::CreatePhysicsPlaneShape(std::get<0>(planeData), std::get<1>(planeData));
   }

   /*-------------------- Create physics compound shape --------------*/
   PhysicsShapeBase *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<> &noData)
   {
      return EngineObjectCreator::CreatePhysicsCompoundShape();
   }

   /*-------------------- Add child shape to compound --------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple</*compound shape*/ PhysicsShapeBase *,
                                                                                    /*child shape*/ PhysicsShapeBase *, glm::vec3 /*child translation*/, glm::vec3 /*child rotation*/> &data)
   {
      assert(std::get<0>(data));
      assert(std::get<1>(data));
      EngineObjectCreator::AddChildShapeToCompoundShape(std::get<0>(data), std::get<1>(data), std::get<2>(data), std::get<3>(data));
   }

   /*-------------------- Create rigid body controller--------------*/
   PhysicsDescriptor *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<PhysicsShapeBase *, std::string, float> descData)
   {
      PhysicsDescriptor *descriptor = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         descriptor = EngineObjectCreator::CreateRigidBodyController(scene->GetPhysicsWorld(), std::get<0>(descData), std::get<1>(descData), std::get<2>(descData));
         scene->GetPhysicsWorld()->AddPhysDescriptor(descriptor);
      }

      return descriptor;
   }

   /*-------------------- Create dynamic character controller--------------*/
   PhysicsDescriptor *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<float, float, float, float> descData)
   {
      PhysicsDescriptor *descriptor = nullptr;

      if (auto scene = mSceneWP.lock())
      {
         descriptor = EngineObjectCreator::CreateDynamicCharacterController(scene->GetPhysicsWorld(), std::get<0>(descData), std::get<1>(descData),
                                                                            std::get<2>(descData), std::get<3>(descData));
         scene->GetPhysicsWorld()->AddPhysDescriptor(descriptor);
      }

      return descriptor;
   }

   /* -------------------  Create Tweener ----------------------------*/
   Tweener *LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<Actor *, std::string> &tweenerData)
   {
      Tweener *createdTweener = nullptr;

      TweenerParser fsmParser;
      const auto &tweener = fsmParser.ParseTweenerDescriptor(
          std::get<1>(tweenerData));
      Actor *actor = std::get<0>(tweenerData);

      assert(actor && tweener);

      actor->AttachTweener(tweener);

      createdTweener = tweener.get();

      return createdTweener;
   }

   /* -------------------  Set tweener bindings ------------------------*/
   void LuaScriptExecutor_EngineObjectsCreator::ExecuteLuaCallback(const std::tuple<Tweener *, std::string, std::string, std::string> &tweenerData)
   {
      auto tweener = std::get<0>(tweenerData);
      assert(tweener);

      if (auto scene = mSceneWP.lock())
      {
         GameObject *gameObject = scene->GetGameObjectByName(std::get<1>(tweenerData));
         const auto &binding = tweener->GetPropertyBindingByName(std::get<2>(tweenerData));
         BindingAttachmentBuilder::SetAttachment(gameObject, binding.get(), std::get<3>(tweenerData));
      }
   }
}

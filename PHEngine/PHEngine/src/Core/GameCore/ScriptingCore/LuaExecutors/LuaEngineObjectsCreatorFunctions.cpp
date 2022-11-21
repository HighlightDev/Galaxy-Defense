#include "LuaEngineObjectsCreatorFunctions.h"
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
#include "Core/GameCore/HumanoidPlayerController.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

using namespace Graphics;
using namespace EngineUtility;
using namespace IO;
using namespace Resources;
using namespace EngineCore::Scripts;

namespace EngineCore
{
   namespace Scripts
   {
      LuaEngineObjectsCreatorFunctions::LuaEngineObjectsCreatorFunctions(const std::string &scriptName)
          : LuaCommonEngineFunctions(scriptName), mActiveComponents(), mAllocatedComponentData()
      {
      }

      LuaEngineObjectsCreatorFunctions::~LuaEngineObjectsCreatorFunctions()
      {
         for (auto dataPtr : mAllocatedComponentData)
         {
            delete dataPtr;
         }
      }

      void LuaEngineObjectsCreatorFunctions::RegisterCallbacks()
      {
         LuaCommonEngineFunctions::RegisterCallbacks();

         using LuaExecutor_t = LuaEngineObjectsCreatorFunctions;

         AddFunctor(7, WrapFunctorIntoAny<void(std::tuple<std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::LoadResourcesAsync, this, std::placeholders::_1)));
         AddFunctor(8, WrapFunctorIntoAny<Component *(std::tuple<std::string, ComponentData *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateComponent, this, std::placeholders::_1)));
         AddFunctor(9, WrapFunctorIntoAny<Actor *(std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateActor, this, std::placeholders::_1)));
         AddFunctor(10, WrapFunctorIntoAny<void(std::tuple<std::string, glm::ivec4, float, float, float, glm::vec3, int32_t>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateThirdPersonCamera, this, std::placeholders::_1)));
         AddFunctor(11, WrapFunctorIntoAny<void(std::tuple<std::string, glm::ivec4, float, float, glm::vec3, int32_t>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateFirstPersonCamera, this, std::placeholders::_1)));
         AddFunctor(12, WrapFunctorIntoAny<void(std::tuple<std::string, Component *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::AttachComponentToActor, this, std::placeholders::_1)));
         AddFunctor(13, WrapFunctorIntoAny<void(std::tuple<Actor *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::AttachPlayerControllerToActor, this, std::placeholders::_1)));
         AddFunctor(14, WrapFunctorIntoAny<ProjectedShadowInfo *(std::tuple<int32_t, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateLightProjectionShadowInfo, this, std::placeholders::_1)));
         AddFunctor(15, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateDirLightComponentData, this, std::placeholders::_1)));
         AddFunctor(16, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, ProjectedShadowInfo *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePointLightComponentData, this, std::placeholders::_1)));
         AddFunctor(17, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, float, ProjectedShadowInfo *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateSpotlightComponentData, this, std::placeholders::_1)));
         AddFunctor(18, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateMeshComponentData, this, std::placeholders::_1)));
         AddFunctor(19, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateSimpleMeshComponentData, this, std::placeholders::_1)));
         AddFunctor(20, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, PhysicsDescriptor *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePhysicsComponentData, this, std::placeholders::_1)));
         AddFunctor(21, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateInputComponentData, this, std::placeholders::_1)));
         AddFunctor(22, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, glm::vec3, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateCharacterMovementComponentData, this, std::placeholders::_1)));
         AddFunctor(23, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePlatformTraverseComponentData, this, std::placeholders::_1)));
         AddFunctor(24, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, glm::vec3, IMaterial *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateSkyboxComponentData, this, std::placeholders::_1)));
         AddFunctor(25, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial *>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateWaterPlaneComponentData, this, std::placeholders::_1)));
         AddFunctor(26, WrapFunctorIntoAny<ComponentData *(std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, std::string /*Camera name*/, glm::ivec4>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePlanarReflectionComponentData, this, std::placeholders::_1)));
         AddFunctor(27, WrapFunctorIntoAny<IMaterial *(std::tuple<std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateMaterial, this, std::placeholders::_1)));
         AddFunctor(28, WrapFunctorIntoAny<void(std::tuple<IMaterial *, std::string, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::SetTextureToMaterial, this, std::placeholders::_1)));
         AddFunctor(29, WrapFunctorIntoAny<void(std::tuple<IMaterial *, float, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::SetFloatToMaterial, this, std::placeholders::_1)));
         AddFunctor(30, WrapFunctorIntoAny<void(std::tuple<IMaterial *, std::string, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::SetDeferredTextureToMaterial, this, std::placeholders::_1)));
         AddFunctor(31, WrapFunctorIntoAny<void(std::tuple<IMaterial *, std::string, std::string, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::SetBindingToMaterial, this, std::placeholders::_1)));
         AddFunctor(32, WrapFunctorIntoAny<PhysicsShapeBase *(std::tuple<glm::vec3>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePhysicsBoxShape, this, std::placeholders::_1)));
         AddFunctor(33, WrapFunctorIntoAny<PhysicsShapeBase *(std::tuple<double>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePhysicsSphereShape, this, std::placeholders::_1)));
         AddFunctor(34, WrapFunctorIntoAny<PhysicsShapeBase *(std::tuple<double, double>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePhysicsCapsuleShape, this, std::placeholders::_1)));
         AddFunctor(35, WrapFunctorIntoAny<PhysicsShapeBase *(std::tuple<glm::vec3, double>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePhysicsPlaneShape, this, std::placeholders::_1)));
         AddFunctor(36, WrapFunctorIntoAny<PhysicsShapeBase *(std::tuple<>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreatePhysicsCompoundShape, this, std::placeholders::_1)));
         AddFunctor(37, WrapFunctorIntoAny<void(std::tuple<PhysicsShapeBase *, PhysicsShapeBase *, glm::vec3, glm::vec3>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::AddCompoundChildShape, this, std::placeholders::_1)));
         AddFunctor(38, WrapFunctorIntoAny<PhysicsDescriptor *(std::tuple<PhysicsShapeBase *, std::string, float>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateRigidBodyController, this, std::placeholders::_1)));
         AddFunctor(39, WrapFunctorIntoAny<PhysicsDescriptor *(std::tuple<float, float, float, float>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateDynamicCharacterController, this, std::placeholders::_1)));
         AddFunctor(40, WrapFunctorIntoAny<Tweener *(std::tuple<Actor *, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::CreateTweener, this, std::placeholders::_1)));
         AddFunctor(41, WrapFunctorIntoAny<void(std::tuple<Tweener *, std::string, std::string, std::string>)>::GetWrappedFunctor(std::bind(&LuaEngineObjectsCreatorFunctions::SetTweenerBinding, this, std::placeholders::_1)));

         LuaCallbackBinder<LuaExecutor_t, 7, void(std::string)>::Bind(mLuaInstance, "_LoadResourcesAsync");
         LuaCallbackBinder<LuaExecutor_t, 8, Component *(std::string, ComponentData *)>::Bind(mLuaInstance, "_CreateComponent");
         LuaCallbackBinder<LuaExecutor_t, 9, Actor *(std::string, glm::vec3, glm::vec3, glm::vec3)>::Bind(mLuaInstance, "_CreateActor");
         LuaCallbackBinder<LuaExecutor_t, 10, void(std::string, glm::ivec4, float, float, float, glm::vec3, int32_t)>::Bind(mLuaInstance, "_CreateThirdPersonCamera");
         LuaCallbackBinder<LuaExecutor_t, 11, void(std::string, glm::ivec4, float, float, glm::vec3, int32_t)>::Bind(mLuaInstance, "_CreateFirstPersonCamera");
         LuaCallbackBinder<LuaExecutor_t, 12, void(std::string, Component *)>::Bind(mLuaInstance, "_AttachComponentToActor");
         LuaCallbackBinder<LuaExecutor_t, 13, void(Actor *)>::Bind(mLuaInstance, "_AttachPlayerControllerToActor");
         LuaCallbackBinder<LuaExecutor_t, 14, ProjectedShadowInfo *(int32_t, std::string)>::Bind(mLuaInstance, "_CreateLightProjectionShadowInfo");
         /*************************************COMPONENT DATA**************************************/
         LuaCallbackBinder<LuaExecutor_t, 15, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo *)>::Bind(mLuaInstance, "_CreateDirLightComponentData");
         LuaCallbackBinder<LuaExecutor_t, 16, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, ProjectedShadowInfo *)>::Bind(mLuaInstance, "_CreatePointLightComponentData");
         LuaCallbackBinder<LuaExecutor_t, 17, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, float, ProjectedShadowInfo *)>::Bind(mLuaInstance, "_CreateSpotlightComponentData");
         LuaCallbackBinder<LuaExecutor_t, 18, ComponentData *(std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *)>::Bind(mLuaInstance, "_CreateMeshComponentData");
         LuaCallbackBinder<LuaExecutor_t, 19, ComponentData *(std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *)>::Bind(mLuaInstance, "_CreateSimpleMeshComponentData");
         LuaCallbackBinder<LuaExecutor_t, 20, ComponentData *(std::string, PhysicsDescriptor *)>::Bind(mLuaInstance, "_CreatePhysicsComponentData");
         LuaCallbackBinder<LuaExecutor_t, 21, ComponentData *(std::string)>::Bind(mLuaInstance, "_CreateInputComponentData");
         LuaCallbackBinder<LuaExecutor_t, 22, ComponentData *(std::string, glm::vec3, std::string)>::Bind(mLuaInstance, "_CreateCharacterMovementComponentData");
         LuaCallbackBinder<LuaExecutor_t, 23, ComponentData *(std::string, std::string)>::Bind(mLuaInstance, "_CreatePlatformTraverseComponentData");
         LuaCallbackBinder<LuaExecutor_t, 24, ComponentData *(std::string, glm::vec3, IMaterial *)>::Bind(mLuaInstance, "_CreateSkyboxComponentData");
         LuaCallbackBinder<LuaExecutor_t, 25, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial *)>::Bind(mLuaInstance, "_CreateWaterPlaneComponentData");
         LuaCallbackBinder<LuaExecutor_t, 26, ComponentData *(std::string, glm::vec3, glm::vec3, glm::vec3, std::string /*Camera name*/, glm::ivec4)>::Bind(mLuaInstance, "_CreatePlanarReflectionComponentData");
         /*************************************COMPONENT DATA**************************************/
         LuaCallbackBinder<LuaExecutor_t, 27, IMaterial *(std::string)>::Bind(mLuaInstance, "_CreateMaterial");
         LuaCallbackBinder<LuaExecutor_t, 28, void(IMaterial *, std::string, std::string)>::Bind(mLuaInstance, "_SetTextureToMaterial");
         LuaCallbackBinder<LuaExecutor_t, 29, void(IMaterial *, float, std::string)>::Bind(mLuaInstance, "_SetFloatToMaterial");
         LuaCallbackBinder<LuaExecutor_t, 30, void(IMaterial *, std::string, std::string)>::Bind(mLuaInstance, "_SetDeferredTextureToMaterial");
         LuaCallbackBinder<LuaExecutor_t, 31, void(IMaterial *, std::string, std::string, std::string)>::Bind(mLuaInstance, "_SetBindingToMaterial");
         /**********************************************PHYSICS COLLISION SHAPE***********************************************/
         LuaCallbackBinder<LuaExecutor_t, 32, PhysicsShapeBase *(glm::vec3)>::Bind(mLuaInstance, "_CreatePhysicsBoxShape");
         LuaCallbackBinder<LuaExecutor_t, 33, PhysicsShapeBase *(double)>::Bind(mLuaInstance, "_CreatePhysicsSphereShape");
         LuaCallbackBinder<LuaExecutor_t, 34, PhysicsShapeBase *(double, double)>::Bind(mLuaInstance, "_CreatePhysicsCapsuleShape");
         LuaCallbackBinder<LuaExecutor_t, 35, PhysicsShapeBase *(glm::vec3, double)>::Bind(mLuaInstance, "_CreatePhysicsPlaneShape");
         LuaCallbackBinder<LuaExecutor_t, 36, PhysicsShapeBase *()>::Bind(mLuaInstance, "_CreatePhysicsCompoundShape");
         LuaCallbackBinder<LuaExecutor_t, 37, void(PhysicsShapeBase *, PhysicsShapeBase *, glm::vec3, glm::vec3)>::Bind(mLuaInstance, "_AddCompoundChildShape");
         /**********************************************PHYSICS COLLISION SHAPE***********************************************/
         LuaCallbackBinder<LuaExecutor_t, 38, PhysicsDescriptor *(PhysicsShapeBase *, std::string, float)>::Bind(mLuaInstance, "_CreateRigidBodyController");
         LuaCallbackBinder<LuaExecutor_t, 39, PhysicsDescriptor *(float, float, float, float)>::Bind(mLuaInstance, "_CreateDynamicCharacterController");
         LuaCallbackBinder<LuaExecutor_t, 40, Tweener *(Actor *, std::string)>::Bind(mLuaInstance, "_CreateTweener");
         LuaCallbackBinder<LuaExecutor_t, 41, void(Tweener *, std::string, std::string, std::string)>::Bind(mLuaInstance, "_SetTweenerBinding");
      }

      void LuaEngineObjectsCreatorFunctions::RunScript()
      {
         LuaCommonEngineFunctions::RunScript();
      }

      /* -------------------  Load asynchronously resources by names ----------------------------*/
      void LuaEngineObjectsCreatorFunctions::LoadResourcesAsync(const std::tuple<std::string> &asyncLoadNamesData)
      {
         const std::string &resourcesNamesStr = std::get<0>(asyncLoadNamesData);
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
      Actor *LuaEngineObjectsCreatorFunctions::CreateActor(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3> &actorData)
      {
         Actor *createdActor = nullptr;

         if (auto scene = mSceneWP.lock())
         {
            auto actorSP = EngineObjectCreator::CreateActorByString(std::get<0>(actorData),
                                                                    std::make_shared<EngineCore::SceneComponent>(std::get<0>(actorData) + "rootComponent", std::get<1>(actorData),
                                                                                                                 std::get<2>(actorData), std::get<3>(actorData)));
            scene->AddActor(actorSP);
            createdActor = actorSP.get();
         }

         return createdActor;
      }

      /* -------------------  Attach component to Actor ----------------------------*/
      void LuaEngineObjectsCreatorFunctions::AttachComponentToActor(const std::tuple<std::string /*actor name*/, Component *> &dataToAttachActorToComponent)
      {
         const std::string &actorName = std::get<0>(dataToAttachActorToComponent);
         Component *component = std::get<1>(dataToAttachActorToComponent);

         const auto &spScene = mSceneWP.lock();
         assert(spScene);

         const auto &actor = spScene->GetActorByName(actorName);
         assert((actor && component, "Actor or component is null"));

         const bool componentExists = mActiveComponents.count(component->GetObjectId());
         assert((componentExists, "Sought component doesn't exist"));

         actor->AddComponent(mActiveComponents[component->GetObjectId()]);
      }

      /* -------------------  Create first person camera ----------------------------*/
      void LuaEngineObjectsCreatorFunctions::CreateFirstPersonCamera(const std::tuple<std::string /*cameraName*/,
                                                                                      glm::ivec4 /*viewPort*/,
                                                                                      float /*initPitchDeg*/,
                                                                                      float /*initYawDeg*/,
                                                                                      glm::vec3 /*init camera position*/,
                                                                                      int32_t /*is main camera in the scene*/> &cameraData)
      {
         if (auto scene = mSceneWP.lock())
         {
            const glm::ivec4 &viewPortData = std::get<1>(cameraData);
            const bool bIsMainSceneCamera = static_cast<int32_t>(std::get<5>(cameraData));

            const auto &firstPersonCamera = EngineObjectCreator::CreateFirstPersonCamera(
                std::get<0>(cameraData),
                scene,
                ViewPortInfo(viewPortData.x, viewPortData.y, viewPortData.z, viewPortData.w),
                std::get<2>(cameraData),
                std::get<3>(cameraData),
                std::get<4>(cameraData),
                bIsMainSceneCamera);

            scene->RegisterCamera(firstPersonCamera);
         }
      }

      /* -------------------  Create third person camera ----------------------------*/
      void LuaEngineObjectsCreatorFunctions::CreateThirdPersonCamera(const std::tuple<std::string /*cameraName*/,
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

      /* ------------------- Attach player controller to actor ----------------------------*/
      void LuaEngineObjectsCreatorFunctions::AttachPlayerControllerToActor(const std::tuple<Actor *> &actorData)
      {
         Actor *actor = std::get<0>(actorData);
         assert((actor, "Actor is null"));

         if (auto scene = mSceneWP.lock())
         {
            auto camera = scene->GetMainCamera();

            auto actorIt = std::find_if(scene->GetActors().begin(), scene->GetActors().end(),
                                        [&](const std::shared_ptr<Actor> &sceneActor)
                                        { return sceneActor->GetObjectId() == actor->GetObjectId(); });
            assert(actorIt != scene->GetActors().end());

            scene->AddActorController(std::make_shared<HumanoidPlayerController>(camera, (*actorIt)));

            if (eCameraType::MAIN_THIRD_PERSON_CAMERA == camera->GetCameraType())
            {
               std::static_pointer_cast<ThirdPersonCamera>(camera)->SetThirdPersonTargetDeferred((*actorIt)->GetGameObjectName());
            }
         }
      }

      /* -------------------  Create component ----------------------------*/
      Component *LuaEngineObjectsCreatorFunctions::CreateComponent(const std::tuple<std::string, ComponentData *> &componentData)
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
      ComponentData *LuaEngineObjectsCreatorFunctions::CreateMeshComponentData(const std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *> &meshComponentData)
      {
         const auto &meshModelName = std::get<1>(meshComponentData);
         auto dataPtr = EngineObjectCreator::CreateMeshComponentData(std::get<0>(meshComponentData),
                                                                     meshModelName, std::get<2>(meshComponentData),
                                                                     std::get<3>(meshComponentData), std::get<4>(meshComponentData), std::get<5>(meshComponentData), std::get<6>(meshComponentData));

         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create simple mesh component data ----------------------------*/
      ComponentData *LuaEngineObjectsCreatorFunctions::CreateSimpleMeshComponentData(const std::tuple<std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string, IMaterial *> &meshComponentData)
      {
         auto dataPtr = EngineObjectCreator::CreateSimpleMeshComponentData(std::get<0>(meshComponentData), std::get<1>(meshComponentData), std::get<2>(meshComponentData),
                                                                           std::get<3>(meshComponentData), std::get<4>(meshComponentData), std::get<5>(meshComponentData), std::get<6>(meshComponentData));

         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create water plane component data ----------------------------*/
      ComponentData *LuaEngineObjectsCreatorFunctions::CreateWaterPlaneComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, IMaterial *> &data)
      {
         auto dataPtr = EngineObjectCreator::CreateWaterPlaneComponentData(std::get<0>(data), std::get<1>(data), std::get<2>(data),
                                                                           std::get<3>(data), std::get<4>(data));

         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create planar reflection component data ----------------------------*/
      ComponentData *LuaEngineObjectsCreatorFunctions::CreatePlanarReflectionComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, std::string /*Camera name*/, glm::ivec4> &data)
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
      ComponentData *LuaEngineObjectsCreatorFunctions::CreateDirLightComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, ProjectedShadowInfo *> &dirLightComponentData)
      {
         auto dataPtr = EngineObjectCreator::CreateDirLightComponentData(std::get<0>(dirLightComponentData), std::get<1>(dirLightComponentData), std::get<2>(dirLightComponentData),
                                                                         std::get<3>(dirLightComponentData), std::get<4>(dirLightComponentData), std::get<5>(dirLightComponentData), std::get<6>(dirLightComponentData));

         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create point light component data ----------------------------*/
      ComponentData *LuaEngineObjectsCreatorFunctions::CreatePointLightComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, ProjectedShadowInfo *> &pointLightComponentData)
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
      ComponentData *LuaEngineObjectsCreatorFunctions::CreateSpotlightComponentData(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3,
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
      ComponentData *LuaEngineObjectsCreatorFunctions::CreatePhysicsComponentData(const std::tuple<std::string, PhysicsDescriptor *> &phyComponentData)
      {
         auto dataPtr = EngineObjectCreator::CreatePhysicsComponentData(std::get<0>(phyComponentData), std::get<1>(phyComponentData));
         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create input component data ----------------------------*/
      ComponentData *LuaEngineObjectsCreatorFunctions::CreateInputComponentData(const std::tuple<std::string> &inputComponentData)
      {
         auto dataPtr = EngineObjectCreator::CreateInputComponentData(std::get<0>(inputComponentData));
         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create character movement component data ----------------------------*/
      ComponentData *LuaEngineObjectsCreatorFunctions::CreateCharacterMovementComponentData(const std::tuple<std::string, glm::vec3, std::string> &movementComponentData)
      {
         auto dataPtr = EngineObjectCreator::CreateCharacterMovementComponentData(std::get<0>(movementComponentData), std::get<1>(movementComponentData), std::get<2>(movementComponentData));
         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create platform movement component data ----------------------------*/
      ComponentData *LuaEngineObjectsCreatorFunctions::CreatePlatformTraverseComponentData(const std::tuple<std::string, std::string> &platformMovementComponentData)
      {
         auto dataPtr = EngineObjectCreator::CreatePlatformTraverseComponentData(std::get<0>(platformMovementComponentData), std::get<1>(platformMovementComponentData));
         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create skybox component data ----------------------------*/
      ComponentData *LuaEngineObjectsCreatorFunctions::CreateSkyboxComponentData(const std::tuple<std::string, glm::vec3, IMaterial *> &skyboxComponentData)
      {
         auto dataPtr = EngineObjectCreator::CreateSkyboxComponentData(std::get<0>(skyboxComponentData), std::get<1>(skyboxComponentData), std::get<2>(skyboxComponentData));
         mAllocatedComponentData.push_back(dataPtr);
         return dataPtr;
      }

      /* -------------------  Create light projection shadow info --------------------*/
      ProjectedShadowInfo *LuaEngineObjectsCreatorFunctions::CreateLightProjectionShadowInfo(const std::tuple<int32_t, std::string> &lightProjectionData)
      {
         ProjectedShadowInfo *shadowProjInfo = nullptr;

         const int32_t shadowAtlasSize = std::get<0>(lightProjectionData);
         const std::string &lightType = std::get<1>(lightProjectionData);

         shadowProjInfo = EngineObjectCreator::CreateProjectedShadowInfo(lightType, glm::ivec2(shadowAtlasSize, shadowAtlasSize));

         return shadowProjInfo;
      }

      /* -------------------  Create material --------------------*/
      IMaterial *LuaEngineObjectsCreatorFunctions::CreateMaterial(const std::tuple<std::string> &buildMaterial)
      {
         MaterialParser materialParser;
         const std::string &materialName = std::get<0>(buildMaterial);
         return materialParser.ParseMaterialDescriptor(materialName);
      }

      /* -------------------  Set texture --------------------*/
      void LuaEngineObjectsCreatorFunctions::SetTextureToMaterial(const std::tuple<IMaterial *, std::string, std::string> &setTextureToMaterial)
      {
         IMaterial *material = std::get<0>(setTextureToMaterial);
         const std::string &textureNames = std::get<1>(setTextureToMaterial);
         const std::string &propertyName = std::get<2>(setTextureToMaterial);
         const auto &texture = TexturePool::GetInstance()->GetOrAllocateResource(textureNames);
         MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, texture);
      }

      /* -------------------  Set deferred texture --------------------*/
      void LuaEngineObjectsCreatorFunctions::SetDeferredTextureToMaterial(const std::tuple<IMaterial *, /*deferred resource creator name*/ std::string, /*property name*/ std::string> &data)
      {
         IMaterial *material = std::get<0>(data);
         const std::string &resourceCreatorName = std::get<1>(data);
         const std::string &propertyName = std::get<2>(data);

         if (auto scene = mSceneWP.lock())
         {
            const auto resourceCreator = scene->GetDeferredResourceCreatorByName(resourceCreatorName);
            assert(resourceCreator);
            MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, resourceCreator);
         }
      }

      /* -------------------  Set float --------------------*/
      void LuaEngineObjectsCreatorFunctions::SetFloatToMaterial(const std::tuple<IMaterial *, float, std::string> &setFloatValueToMaterial)
      {
         IMaterial *material = std::get<0>(setFloatValueToMaterial);
         float value = std::get<1>(setFloatValueToMaterial);
         const std::string &propertyName = std::get<2>(setFloatValueToMaterial);
         MaterialPropertySetter::SetMaterialPropertyValue(material, propertyName, value);
      }

      /* -------------------  Set binding to material --------------------*/
      void LuaEngineObjectsCreatorFunctions::SetBindingToMaterial(const std::tuple<IMaterial *, std::string, std::string, std::string> &setBindingToMaterial)
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
      PhysicsShapeBase *LuaEngineObjectsCreatorFunctions::CreatePhysicsSphereShape(const std::tuple<double> &value)
      {
         return EngineObjectCreator::CreatePhysicsSphereShape(std::get<0>(value));
      }

      /*-------------------- Create physics collision box shape --------------*/
      PhysicsShapeBase *LuaEngineObjectsCreatorFunctions::CreatePhysicsBoxShape(const std::tuple<glm::vec3> &halfExtent)
      {
         return EngineObjectCreator::CreatePhysicsBoxShape(std::get<0>(halfExtent));
      }

      /*-------------------- Create physics collision capsule shape --------------*/
      PhysicsShapeBase *LuaEngineObjectsCreatorFunctions::CreatePhysicsCapsuleShape(const std::tuple<double, double> &capsuleData)
      {
         return EngineObjectCreator::CreatePhysicsCapsuleShape(std::get<0>(capsuleData), std::get<1>(capsuleData));
      }

      /*-------------------- Create physics collision plane shape --------------*/
      PhysicsShapeBase *LuaEngineObjectsCreatorFunctions::CreatePhysicsPlaneShape(const std::tuple<glm::vec3, double> planeData)
      {
         return EngineObjectCreator::CreatePhysicsPlaneShape(std::get<0>(planeData), std::get<1>(planeData));
      }

      /*-------------------- Create physics compound shape --------------*/
      PhysicsShapeBase *LuaEngineObjectsCreatorFunctions::CreatePhysicsCompoundShape(const std::tuple<> &noData)
      {
         return EngineObjectCreator::CreatePhysicsCompoundShape();
      }

      /*-------------------- Add child shape to compound --------------*/
      void LuaEngineObjectsCreatorFunctions::AddCompoundChildShape(const std::tuple</*compound shape*/ PhysicsShapeBase *,
                                                                                    /*child shape*/ PhysicsShapeBase *, glm::vec3 /*child translation*/, glm::vec3 /*child rotation*/> &data)
      {
         assert(std::get<0>(data));
         assert(std::get<1>(data));
         EngineObjectCreator::AddChildShapeToCompoundShape(std::get<0>(data), std::get<1>(data), std::get<2>(data), std::get<3>(data));
      }

      /*-------------------- Create rigid body controller--------------*/
      PhysicsDescriptor *LuaEngineObjectsCreatorFunctions::CreateRigidBodyController(const std::tuple<PhysicsShapeBase *, std::string, float> descData)
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
      PhysicsDescriptor *LuaEngineObjectsCreatorFunctions::CreateDynamicCharacterController(const std::tuple<float, float, float, float> descData)
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
      Tweener *LuaEngineObjectsCreatorFunctions::CreateTweener(const std::tuple<Actor *, std::string> &tweenerData)
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
      void LuaEngineObjectsCreatorFunctions::SetTweenerBinding(const std::tuple<Tweener *, std::string, std::string, std::string> &tweenerData)
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
}

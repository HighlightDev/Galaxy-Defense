#include "EngineObjectCreator.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/PlatformTraverseComponent.h"
#include "Core/GameCore/Components/HumanoidPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"
#include "Core/GameCore/Components/ComponentData/SpotlightComponentData.h"
#include "Core/GameCore/Components/SpotlightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/WaterPlaneComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionPlaneShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCompoundShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Components/PhysicsComponents/RigidBodyPhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedSpotlightShadowInfo.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/CubemapComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/LightComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PlanarReflectionComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PlatformTraverseComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/SkeletalMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/SkyboxComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/WaterPlaneComponentCreator.h"

using namespace EngineUtility;

namespace EngineCore
{
   EngineObjectCreator::EngineObjectCreator()
   {
      mDefaultComponentNames = {
          "LightComponent",
          "DirectionalLightComponent",
          "SpotlightComponent",
          "StaticMeshComponent",
          "SkeletalMeshComponent",
          "RigidBodyPhysicsComponent",
          "CharacterPhysicsComponent",
          "HumanoidPhysicsMovementComponent",
          "PlatformTraverseComponent",
          "SkyboxComponent",
          "WaterPlaneComponent",
          "PlanarReflectionComponent",
          "InputComponent"};
   }

   void EngineObjectCreator::SetScene(const std::weak_ptr<Scene> &scene)
   {
      mSceneWp = scene;
   }

   void EngineObjectCreator::RegisterActorCreatorFactory(const std::string &factoryKey, const std::shared_ptr<IEngineActorCreatorFactory> &creatorFactoryInstance)
   {
      assert(!mActorCreatorFactoriesMap.count(factoryKey));
      mActorCreatorFactoriesMap[factoryKey] = creatorFactoryInstance;
   }

   void EngineObjectCreator::RegisterComponentCreatorFactory(const std::string &factoryKey, const std::shared_ptr<IEngineComponentCreatorFactory> &creatorFactoryInstance)
   {
      assert(!mComponentCreatorFactoriesMap.count(factoryKey));
      mComponentCreatorFactoriesMap[factoryKey] = creatorFactoryInstance;
   }

   // Actor will be created and added to the scene
   int32_t EngineObjectCreator::CreateActor(const std::string &actorType,
                                            const std::string &actorName,
                                            const glm::vec3 &rootTranslation,
                                            const glm::vec3 &rootEulerRotation,
                                            const glm::vec3 &rootScale,
                                            const std::string &jsonParamStr) const
   {
      assert(mActorCreatorFactoriesMap.count(actorType));
      return mActorCreatorFactoriesMap.at(actorType)->CreateActor(mSceneWp, actorName, rootTranslation, rootEulerRotation, rootScale, jsonParamStr);
   }

   void EngineObjectCreator::CreateComponent(const int32_t actorObjectId,
                                             const std::string &componentType,
                                             const std::string &componentDataJsonStr) const
   {
      std::string factoryName = "";
      if (mDefaultComponentNames.count(componentType))
      {
         factoryName = "DefaultComponentCreatorFactory";
      }
      else
      {
         factoryName = componentType;
      }
      assert(mComponentCreatorFactoriesMap.count(factoryName));
      mComponentCreatorFactoriesMap.at(factoryName)->CreateComponent(mSceneWp, actorObjectId, componentType, componentDataJsonStr);
   }

   void EngineObjectCreator::CreateThirdPersonCamera(const std::string &cameraName,
                                                     const ViewPortInfo &viewPort,
                                                     const float initPitchDeg,
                                                     const float initYawDeg,
                                                     const float camDistanceToThirdPersonTarget,
                                                     const glm::vec3 &thirdPersonTargetOffset,
                                                     const bool bIsMainSceneCamera)
   {
      if (const auto &sceneSp = mSceneWp.lock())
      {
         const eCameraType cameraType = bIsMainSceneCamera ? eCameraType::MAIN_THIRD_PERSON_CAMERA : eCameraType::SECONDARY_THIRD_PERSON_CAMERA;
         const auto camera = std::make_shared<ThirdPersonCamera>(cameraName, cameraType, sceneSp, viewPort,
                                                                 initPitchDeg, initYawDeg, camDistanceToThirdPersonTarget, thirdPersonTargetOffset);

         if (bIsMainSceneCamera)
         {
            sceneSp->RegisterMainCamera(camera);
         }
         else
         {
            sceneSp->RegisterCamera(camera);
         }
      }
   }

   void EngineObjectCreator::CreateFirstPersonCamera(const std::string &cameraName,
                                                     const ViewPortInfo &viewPort,
                                                     const float initPitchDeg,
                                                     const float initYawDeg,
                                                     const glm::vec3 &cameraPosition,
                                                     const bool bIsMainSceneCamera)
   {
      if (const auto &sceneSp = mSceneWp.lock())
      {
         const eCameraType cameraType = bIsMainSceneCamera ? eCameraType::MAIN_FIRST_PERSON_CAMERA : eCameraType::SECONDARY_FIRST_PERSON_CAMERA;
         const auto camera = std::make_shared<FirstPersonCamera>(cameraName, cameraType, sceneSp, viewPort, initPitchDeg, initYawDeg, cameraPosition);

         if (bIsMainSceneCamera)
         {
            sceneSp->RegisterMainCamera(camera);
         }
         else
         {
            sceneSp->RegisterCamera(camera);
         }
      }
   }
}

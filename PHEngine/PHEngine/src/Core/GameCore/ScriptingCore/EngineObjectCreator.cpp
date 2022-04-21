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
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyPlaneShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCompoundShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Components/PhysicsComponents/RigidBodyPhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedSpotlightShadowInfo.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace EngineUtility;

namespace EngineCore
{

   ComponentData *EngineObjectCreator::CreateSpotlightComponentData(const std::string &gameObjectName, const glm::vec3 &translation,
                                                                    const glm::vec3 &rotation,
                                                                    const glm::vec3 &ambient, const glm::vec3 &diffuse,
                                                                    const glm::vec3 &specular, const glm::vec3 &attenutation,
                                                                    float radianceRadius, float cutoff, ProjectedShadowInfo *shadowInfo)
   {
      return new SpotlightComponentData(gameObjectName, translation, rotation, attenutation, radianceRadius, cutoff, ambient, diffuse, specular, shadowInfo);
   }

   ComponentData *EngineObjectCreator::CreatePointLightComponentData(const std::string &gameObjectName, const glm::vec3 &translation,
                                                                     const glm::vec3 &ambient,
                                                                     const glm::vec3 &diffuse, const glm::vec3 &specular,
                                                                     const glm::vec3 &attenutation, float radianceRadius,
                                                                     ProjectedShadowInfo *shadowInfo)
   {
      return new PointLightComponentData(gameObjectName, translation, attenutation, radianceRadius, ambient, diffuse, specular, shadowInfo);
   }

   ComponentData *EngineObjectCreator::CreateDirLightComponentData(const std::string &gameObjectName, const glm::vec3 &rotation,
                                                                   const glm::vec3 &direction, const glm::vec3 &ambient,
                                                                   const glm::vec3 &diffuse, const glm::vec3 &specular,
                                                                   ProjectedShadowInfo *shadowInfo)
   {
      return new DirectionalLightComponentData(gameObjectName, rotation, direction, ambient, diffuse, specular, shadowInfo);
   }

   ComponentData *EngineObjectCreator::CreateMeshComponentData(const std::string &gameObjectName, const std::string &relativePathToMesh,
                                                               const glm::vec3 &translation,
                                                               const glm::vec3 &rotation, const glm::vec3 &scale,
                                                               const std::string &luaPathToFile, IMaterial *material)
   {
      return new MeshComponentData(gameObjectName, relativePathToMesh, translation, rotation, scale, luaPathToFile, material);
   }

   ComponentData *EngineObjectCreator::CreateSimpleMeshComponentData(const std::string &gameObjectName, const std::string &simpleMeshType,
                                                                     const glm::vec3 &translation,
                                                                     const glm::vec3 &rotation, const glm::vec3 &scale,
                                                                     const std::string &luaPathToFile, IMaterial *material)
   {
      return new SimpleMeshComponentData(gameObjectName, simpleMeshType, translation, rotation, scale, luaPathToFile, material);
   }

   std::shared_ptr<Actor> EngineObjectCreator::CreateActorByString(const std::string &gameObjectName,
                                                                   std::shared_ptr<SceneComponent> rootComponent)
   {
      return std::make_shared<Actor>(gameObjectName, rootComponent);
   }

   std::shared_ptr<ACamera> EngineObjectCreator::CreateThirdPersonCamera(const std::string &cameraName, std::shared_ptr<Scene> scene,
                                                                         const ViewPortInfo &viewPort,
                                                                         const float initPitchDeg, const float initYawDeg,
                                                                         const float camDistanceToThirdPersonTarget,
                                                                         const glm::vec3 &thirdPersonTargetOffset,
                                                                         const bool bIsMainSceneCamera)
   {
      const eCameraType thirdPersonCameraType = bIsMainSceneCamera ? eCameraType::MAIN_THIRD_PERSON_CAMERA : eCameraType::SECONDARY_THIRD_PERSON_CAMERA;
      return std::make_shared<ThirdPersonCamera>(cameraName, thirdPersonCameraType, scene, viewPort,
                                                 initPitchDeg, initYawDeg, camDistanceToThirdPersonTarget, thirdPersonTargetOffset);
   }

   std::shared_ptr<ACamera> EngineObjectCreator::CreateFirstPersonCamera(const std::string &cameraName, std::shared_ptr<Scene> scene,
                                                                         const ViewPortInfo &viewPort,
                                                                         const float initPitchDeg, const float initYawDeg,
                                                                         const glm::vec3 &cameraPosition, const bool bIsMainSceneCamera)
   {
      const eCameraType thirdPersonCameraType = bIsMainSceneCamera ? eCameraType::MAIN_FIRST_PERSON_CAMERA : eCameraType::SECONDARY_FIRST_PERSON_CAMERA;
      return std::make_shared<FirstPersonCamera>(cameraName, thirdPersonCameraType, scene, viewPort, initPitchDeg, initYawDeg, cameraPosition);
   }

   std::shared_ptr<Component> EngineObjectCreator::CreateComponentByString(const std::string &componentType, ComponentData *componentData,
                                                                           std::shared_ptr<Scene> scene)
   {
      assert(componentData && scene);

      std::shared_ptr<Component> result;

      if ("LightComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<PointLightComponent, eComponentMetaType::LightComponent>(*componentData);
      }
      else if ("DirectionalLightComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<DirectionalLightComponent, EngineCore::eComponentMetaType::LightComponent>(*componentData);
      }
      else if ("SpotlightComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<SpotlightComponent, EngineCore::eComponentMetaType::LightComponent>(*componentData);
      }
      else if ("StaticMeshComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<StaticMeshComponent, eComponentMetaType::StaticMesh>(*componentData);
      }
      else if ("SkeletalMeshComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<SkeletalMeshComponent, eComponentMetaType::SkeletalMesh>(*componentData);
      }
      else if ("RigidBodyPhysicsComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<RigidBodyPhysicsComponent, eComponentMetaType::Physics>(*componentData);
      }
      else if ("CharacterPhysicsComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<CharacterPhysicsComponent, eComponentMetaType::Physics>(*componentData);
      }
      else if ("InputComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<InputComponent, eComponentMetaType::Input>(*componentData);
      }
      else if ("HumanoidPhysicsMovementComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<HumanoidPhysicsMovementComponent, eComponentMetaType::Movement>(*componentData);
      }
      else if ("PlatformTraverseComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<PlatformTraverseComponent, eComponentMetaType::PlatformTraverse>(*componentData);
      }
      else if ("SkyboxComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<SkyboxComponent, eComponentMetaType::Skybox>(*componentData);
      }
      else if ("WaterPlaneComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<WaterPlaneComponent, eComponentMetaType::WaterPlane>(*componentData);
      }
      else if ("PlanarReflectionComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<PlanarReflectionComponent, eComponentMetaType::PlanarReflection>(*componentData);
      }
      else
      {
         assert(false);
      }

      return result;
   }

   ProjectedShadowInfo *EngineObjectCreator::CreateProjectedShadowInfo(const std::string &lightType, const glm::ivec2 &shadowAtlasSize)
   {
      ProjectedShadowInfo *shadowProjInfo = nullptr;
      if (lightType == "point_light")
      {
         auto pointLightTAR = TextureAtlasFactory::GetInstance()->AddTextureCubeAtlasRequest(shadowAtlasSize);
         shadowProjInfo = new ProjectedPointLightShadowInfo(pointLightTAR);
      }
      else if (lightType == "direct_light")
      {
         auto directionalLightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(shadowAtlasSize);
         const auto &cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
         const float orthoHalfExtent = cfg.ShadowOrthoProjectionHalfExtent;
         shadowProjInfo = new ProjectedDirectionalLightShadowInfo(directionalLightTAR, orthoHalfExtent);
      }
      else if (lightType == "spotlight")
      {
         auto spotlightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(shadowAtlasSize);
         shadowProjInfo = new ProjectedSpotlightShadowInfo(spotlightTAR);
      }
      else
      {
         assert(false);
      }
      return shadowProjInfo;
   }

   PhysicsShapeBase *EngineObjectCreator::CreatePhysicsBoxShape(const glm::vec3 &halfExtent)
   {
      return new PhyBoxShape(halfExtent);
   }

   PhysicsShapeBase *EngineObjectCreator::CreatePhysicsCapsuleShape(const double radius, const double height)
   {
      return new PhyCapsuleShape(radius, height);
   }

   PhysicsShapeBase *EngineObjectCreator::CreatePhysicsPlaneShape(const glm::vec3 &normal, const double d)
   {
      return new PhyPlaneShape(normal, d);
   }

   PhysicsShapeBase *EngineObjectCreator::CreatePhysicsSphereShape(const double radius)
   {
      return new PhySphereShape(radius);
   }

   PhysicsShapeBase *EngineObjectCreator::CreatePhysicsCompoundShape()
   {
      return new PhyCompoundShape();
   }

   void EngineObjectCreator::AddChildShapeToCompoundShape(PhysicsShapeBase *compoundShape, PhysicsShapeBase *childShape,
                                                          const glm::vec3 &translation, const glm::vec3 &rotation)
   {
      PhyCompoundShape *mCompoundShape = static_cast<PhyCompoundShape *>(compoundShape);
      assert(mCompoundShape);
      NoScaleEulerRotationTransform childTransform = NoScaleEulerRotationTransform(translation, rotation);
      mCompoundShape->AddChildShape(childTransform, childShape);
   }

   PhysicsDescriptor *EngineObjectCreator::CreateRigidBodyController(PhysicsWorld *physWorld, PhysicsShapeBase *phyShape,
                                                                     const std::string &bodyType, const float mass)
   {
      const ePhysicsBodyType physBodyType = "STATIC_BODY" == bodyType ? ePhysicsBodyType::STATIC : "KINEMATIC_BODY" == bodyType ? ePhysicsBodyType::KINEMATIC
                                                                                                                                : ePhysicsBodyType::DYNAMIC;
      return new RigidBodyController(physWorld, phyShape, physBodyType, mass);
   }

   PhysicsDescriptor *EngineObjectCreator::CreateRigidBodyController(PhysicsWorld *physWorld, PhysicsShapeBase *phyShape, const ePhysicsBodyType &bodyType, const float mass)
   {
      return new RigidBodyController(physWorld, phyShape, bodyType, mass);
   }

   PhysicsDescriptor *EngineObjectCreator::CreateDynamicCharacterController(PhysicsWorld *physWorld, float capsuleRadius, float capsuleHeight,
                                                                            float mass, float stepHeight)
   {
      return new DynamicCharacterController(physWorld, capsuleRadius, capsuleHeight, mass, stepHeight);
   }

   ComponentData *EngineObjectCreator::CreatePhysicsComponentData(const std::string &gameObjectName, PhysicsDescriptor *physDescriptor)
   {
      return new PhysicsComponentData(gameObjectName, physDescriptor);
   }

   ComponentData *EngineObjectCreator::CreateCharacterMovementComponentData(const std::string &gameObjectName,
                                                                            const glm::vec3 &launchDirection, const std::string &cameraName)
   {
      return new HumanoidMovementComponentData(gameObjectName, launchDirection, cameraName);
   }

   ComponentData *EngineObjectCreator::CreatePlatformTraverseComponentData(const std::string &gameObjectName,
                                                                           const std::string &scriptName)
   {
      return new PlatformTraverseComponentData(gameObjectName, scriptName);
   }

   ComponentData *EngineObjectCreator::CreateInputComponentData(const std::string &gameObjectName)
   {
      return new InputComponentData(gameObjectName);
   }

   ComponentData *EngineObjectCreator::CreateSkyboxComponentData(const std::string &gameObjectName,
                                                                 const glm::vec3 &scale, IMaterial *material)
   {
      return new SkyboxComponentData(gameObjectName, scale, material);
   }

   ComponentData *EngineObjectCreator::CreateWaterPlaneComponentData(const std::string &gameObjectName,
                                                                     const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale, IMaterial *materialInstance)
   {
      return new WaterPlaneComponentData(gameObjectName, translation, rotation, scale, materialInstance);
   }

   ComponentData *EngineObjectCreator::CreatePlanarReflectionComponentData(const std::string &gameObjectName,
                                                                           const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale, ACamera *ownerCamera,
                                                                           const ViewPortInfo &fboViewPortInfo)
   {
      return new PlanarReflectionComponentData(gameObjectName, translation, rotation, scale, ownerCamera, fboViewPortInfo);
   }
}

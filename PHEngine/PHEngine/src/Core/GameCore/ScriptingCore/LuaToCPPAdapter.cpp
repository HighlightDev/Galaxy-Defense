#include "LuaToCPPAdapter.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GlobalSettings.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/SpotlightComponentData.h"
#include "Core/GameCore/Components/SpotlightComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyPlaneShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedSpotlightShadowInfo.h"

namespace Game
{

   ComponentData* LuaToCPPAdapter::CreateSpotlightComponentData(const std::string& gameObjectName, const glm::vec3& translation, const glm::vec3& rotation,
      const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& attenutation, float radianceRadius, float cutoff, ProjectedShadowInfo* shadowInfo)
   {
      return new SpotlightComponentData(gameObjectName, translation, rotation, attenutation, radianceRadius, cutoff, ambient, diffuse, specular, shadowInfo);
   }

   ComponentData* LuaToCPPAdapter::CreatePointLightComponentData(const std::string& gameObjectName, const glm::vec3& translation, const glm::vec3& ambient,
      const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& attenutation, float radianceRadius, ProjectedShadowInfo* shadowInfo)
   {
      return new PointLightComponentData(gameObjectName, translation, attenutation, radianceRadius, ambient, diffuse, specular, shadowInfo);
   }

   ComponentData* LuaToCPPAdapter::CreateDirLightComponentData(const std::string& gameObjectName, const glm::vec3& rotation, const glm::vec3& direction, const glm::vec3& ambient,
      const glm::vec3& diffuse, const glm::vec3& specular, ProjectedShadowInfo* shadowInfo)
   {
      return new DirectionalLightComponentData(gameObjectName, rotation, direction, ambient, diffuse, specular, shadowInfo);
   }

   ComponentData* LuaToCPPAdapter::CreateMeshComponentData(const std::string& gameObjectName, const std::string& pathToMesh, const glm::vec3& translation,
      const glm::vec3& rotation, const glm::vec3& scale, const std::string& luaPathToFile, IMaterial* material)
   {
         return new MeshComponentData(gameObjectName, pathToMesh, translation, rotation, scale, luaPathToFile, material);
   }

   std::shared_ptr<Actor> LuaToCPPAdapter::CreateActorByString(const std::string& gameObjectName, std::shared_ptr<SceneComponent> rootComponent)
   {
      return std::make_shared<Actor>(gameObjectName, rootComponent);
   }

   std::shared_ptr<Component> LuaToCPPAdapter::CreateComponentByString(const std::string& componentType, ComponentData* componentData, Scene* scene)
   {
      assert(componentData && scene);

      std::shared_ptr<Component> result;

      if ("PointLightComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::PointLight, PointLightComponent>(*componentData);
      }
      else if ("DirectionalLightComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<Game::ComponentMetaType::DirectionalLight, DirectionalLightComponent>(*componentData);
      }
      else if ("SpotlightComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<Game::ComponentMetaType::Spotlight, SpotlightComponent>(*componentData);
      }
      else if ("StaticMeshComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::StaticMesh, StaticMeshComponent>(*componentData);
      }
      else if ("SkeletalMeshComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::SkeletalMesh, SkeletalMeshComponent>(*componentData);
      }
      else if ("PhysicsComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::Physics, PhysicsComponent>(*componentData);
      }
      else if ("CharacterPhysicsComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::Physics, CharacterPhysicsComponent>(*componentData);
      }
      else if ("InputComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::Input, InputComponent>(*componentData);
      }
      else if ("CharacterMovementComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::CharacterMovement, CharacterMovementComponent>(*componentData);
      }
      else if ("MovementComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::Movement, MovementComponent>(*componentData);
      }
      else if ("SkyboxComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::Skybox, SkyboxComponent>(*componentData);
      }
      else
      {
         assert(false);
      }

      return result;
   }

   ProjectedShadowInfo* LuaToCPPAdapter::CreateProjectedShadowInfo(const std::string& lightType, const glm::ivec2& shadowAtlasSize)
   {
      ProjectedShadowInfo* shadowProjInfo = nullptr;
      if (lightType == "point_light")
      {
         auto pointLightTAR = TextureAtlasFactory::GetInstance()->AddTextureCubeAtlasRequest(shadowAtlasSize);
         shadowProjInfo = new ProjectedPointLightShadowInfo(pointLightTAR);
      }
      else if (lightType == "direct_light")
      {
         auto directionalLightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(shadowAtlasSize);
         const float orthoHalfExtent = GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent();
         shadowProjInfo = new ProjectedDirectionalLightShadowInfo(directionalLightTAR, orthoHalfExtent);
      }
      else if (lightType == "spotlight")
      {
         auto spotlightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(shadowAtlasSize);
         const float orthoHalfExtent = GlobalSettings::GetInstance()->GetShadowOrthoProjectionHalfExtent();
         shadowProjInfo = new ProjectedSpotlightShadowInfo(spotlightTAR);
      }
      else
      {
         assert(false);
      }
      return shadowProjInfo;
   }

   PhysicsShapeBase* LuaToCPPAdapter::CreatePhysicsBoxShape(const glm::vec3& halfExtent)
   {
      return new PhyBoxShape(halfExtent);
   }

   PhysicsShapeBase* LuaToCPPAdapter::CreatePhysicsCapsuleShape(const float radius, const float height)
   {
      return new PhyCapsuleShape(radius, height);
   }

   PhysicsShapeBase* LuaToCPPAdapter::CreatePhysicsPlaneShape(const glm::vec3& normal, const float d)
   {
      return new PhyPlaneShape(normal, d);
   }

   PhysicsShapeBase* LuaToCPPAdapter::CreatePhysicsSphereShape(const float radius)
   {
      return new PhySphereShape(radius);
   }

   PhysicsDescriptor* LuaToCPPAdapter::CreateRigidBodyController(PhysicsWorld* physWorld, PhysicsShapeBase* phyShape, const std::string& bodyType, const float mass)
   {
      const PhysicsBodyType physBodyType = "STATIC_BODY" == bodyType ? PhysicsBodyType::STATIC : "KINEMATIC_BODY" == bodyType ? PhysicsBodyType::KINEMATIC : PhysicsBodyType::DYNAMIC;
      return new RigidBodyController(physWorld, phyShape, physBodyType, mass);
   }

   PhysicsDescriptor* LuaToCPPAdapter::CreateRigidBodyController(PhysicsWorld* physWorld, PhysicsShapeBase* phyShape, const PhysicsBodyType& bodyType, const float mass)
   {
      return new RigidBodyController(physWorld, phyShape, bodyType, mass);
   }

   PhysicsDescriptor* LuaToCPPAdapter::CreateDynamicCharacterController(PhysicsWorld* physWorld, float capsuleRadius, float capsuleHeight,
      float mass, float stepHeight)
   {
      return new DynamicCharacterController(physWorld, capsuleRadius, capsuleHeight, mass, stepHeight);
   }

   ComponentData* LuaToCPPAdapter::CreatePhysicsComponentData(const std::string& gameObjectName, PhysicsDescriptor* physDescriptor)
   {
      return new PhysicsComponentData(gameObjectName, physDescriptor);
   }

   ComponentData* LuaToCPPAdapter::CreateCharacterMovementComponentData(const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName)
   {
      return new CharacterMovementComponentData(gameObjectName, launchDirection, cameraName);
   }

   ComponentData* LuaToCPPAdapter::CreateMovementComponentData(const std::string& gameObjectName, const std::string& scriptName)
   {
      return new MovementComponentData(gameObjectName, scriptName);
   }

   ComponentData* LuaToCPPAdapter::CreateInputComponentData(const std::string& gameObjectName)
   {
      return new InputComponentData(gameObjectName);
   }

   ComponentData* LuaToCPPAdapter::CreateSkyboxComponentData(const std::string& gameObjectName, const glm::vec3& scale, IMaterial* material) 
   {
      return new SkyboxComponentData(gameObjectName, scale, material);
   }

}

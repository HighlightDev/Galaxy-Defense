#include "LuaToCPPAdapter.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyPlaneShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"

namespace Game
{

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
      std::shared_ptr<Component> result;

      if ("DirLightComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<Game::ComponentMetaType::DirectionalLight, DirectionalLightComponent>(*componentData);
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
      else if ("MovementComponent" == componentType)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::Movement, CharacterMovementComponent>(*componentData);
      }
      else assert((false, "Unknown component type."));

      delete componentData;

      return result;
   }

   PhyShapeBase* LuaToCPPAdapter::CreatePhysicsBoxShape(const glm::vec3& halfExtent)
   {
      return new PhyBoxShape(halfExtent);
   }

   PhyShapeBase* LuaToCPPAdapter::CreatePhysicsCapsuleShape(const float radius, const float height)
   {
      return new PhyCapsuleShape(radius, height);
   }

   PhyShapeBase* LuaToCPPAdapter::CreatePhysicsPlaneShape(const glm::vec3& normal, const float d)
   {
      return new PhyPlaneShape(normal, d);
   }

   PhyShapeBase* LuaToCPPAdapter::CreatePhysicsSphereShape(const float radius)
   {
      return new PhySphereShape(radius);
   }

   PhysicsDescriptor* LuaToCPPAdapter::CreateRigidBodyController(PhysicsWorld* physWorld, PhyShapeBase* phyShape, const float mass)
   {
      return new RigidBodyController(physWorld, phyShape, mass);
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

   ComponentData* LuaToCPPAdapter::CreateMovementComponentData(const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName)
   {
      return new MovementComponentData(gameObjectName, launchDirection, cameraName);
   }

   ComponentData* LuaToCPPAdapter::CreateInputComponentData(const std::string& gameObjectName)
   {
      return new InputComponentData(gameObjectName);
   }

}

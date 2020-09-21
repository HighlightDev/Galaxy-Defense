#include "ComponentCreator.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyPlaneShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"

namespace Labyrinth
{

   ComponentData* ComponentCreator::CreateDirLightComponentData(const glm::vec3& rotation, const glm::vec3& direction, const glm::vec3& ambient,
      const glm::vec3& diffuse, const glm::vec3& specular, ProjectedShadowInfo* shadowInfo)
   {
      return new DirectionalLightComponentData(rotation, direction, ambient, diffuse, specular, shadowInfo);
   }

   ComponentData* ComponentCreator::CreateMeshComponentData(const std::string& pathToMesh, const glm::vec3& translation,
      const glm::vec3& rotation, const glm::vec3& scale, IMaterial* material)
   {
         return new MeshComponentData(pathToMesh, translation, rotation, scale, material);
   }

   std::shared_ptr<Component> ComponentCreator::CreateComponentByString(const std::string& componentName, ComponentData* componentData, Scene* scene)
   {
      std::shared_ptr<Component> result;

      if ("DirLightComponent" == componentName)
      {
         result = scene->CreateComponent_GameThread<Game::ComponentMetaType::DirectionalLight, DirectionalLightComponent>(*componentData);
      }
      else if ("StaticMeshComponent" == componentName)
      {
         result = scene->CreateComponent_GameThread<ComponentMetaType::StaticMesh, StaticMeshComponent>(*componentData);
      }
      else if ("SkeletalMeshComponent" == componentName)
      {

      }

      delete componentData;

      return result;
   }

   PhyShapeBase* ComponentCreator::CreatePhysicsBoxShape(const glm::vec3& halfExtent)
   {
      return new PhyBoxShape(halfExtent);
   }

   PhyShapeBase* ComponentCreator::CreatePhysicsCapsuleShape(const float radius, const float height)
   {
      return new PhyCapsuleShape(radius, height);
   }

   PhyShapeBase* ComponentCreator::CreatePhysicsPlaneShape(const glm::vec3& normal, const float d)
   {
      return new PhyPlaneShape(normal, d);
   }

   PhyShapeBase* ComponentCreator::CreatePhysicsSphereShape(const float radius)
   {
      return new PhySphereShape(radius);
   }

}

#pragma once

#include <memory>
#include <string>

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyShapeBase.h"

using namespace Game;
using namespace Graphics;

namespace Game
{
   class Scene;
}

using namespace EnginePhysics;

namespace Labyrinth
{
   using Game::Scene;

   class ComponentCreator
   {
   public:

      static std::shared_ptr<Component> CreateComponentByString(const std::string& componentName, ComponentData* data, class Scene* scene);

      static ComponentData* CreateDirLightComponentData(const glm::vec3& rotation, const glm::vec3& direction, const glm::vec3& ambient,
         const glm::vec3& diffuse, const glm::vec3& specular, ProjectedShadowInfo* shadowInfo);

      static ComponentData* CreateMeshComponentData(const std::string& pathToMesh, const glm::vec3& translation,
         const glm::vec3& rotation, const glm::vec3& scale, IMaterial* material);

      static PhyShapeBase* CreatePhysicsBoxShape(const glm::vec3& halfExtent);
      static PhyShapeBase* CreatePhysicsCapsuleShape(const float radius, const float height);
      static PhyShapeBase* CreatePhysicsPlaneShape(const glm::vec3& normal, const float d);
      static PhyShapeBase* CreatePhysicsSphereShape(const float radius);
   };

}


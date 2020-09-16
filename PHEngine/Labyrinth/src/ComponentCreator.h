#pragma once

#include <memory>
#include <string>

#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"

using namespace Game;
using namespace Graphics;

namespace Game
{
   class Scene;
}

namespace Labyrinth
{
   using Game::Scene;

   class ComponentCreator
   {
   public:

      static std::shared_ptr<Component> CreateComponentByString(const std::string& componentName, ComponentData* data, class Scene* scene);

      static ComponentData* CreateDirLightComponentData(const glm::vec3& rotation, const glm::vec3& direction, const glm::vec3& ambient,
         const glm::vec3& diffuse, const glm::vec3& specular, ProjectedShadowInfo* shadowInfo);
   };

}


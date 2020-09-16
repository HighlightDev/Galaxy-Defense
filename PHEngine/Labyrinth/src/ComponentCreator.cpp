#include "ComponentCreator.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"

namespace Labyrinth
{

   ComponentData* ComponentCreator::CreateDirLightComponentData(const glm::vec3& rotation, const glm::vec3& direction, const glm::vec3& ambient,
      const glm::vec3& diffuse, const glm::vec3& specular, ProjectedShadowInfo* shadowInfo)
   {
      return new DirectionalLightComponentData(rotation, direction, ambient, diffuse, specular, shadowInfo);
   }

   std::shared_ptr<Component> ComponentCreator::CreateComponentByString(const std::string& componentName, ComponentData* componentData, Scene* scene)
   {
      std::shared_ptr<Component> result;

      if ("DirLComponent" == componentName)
      {
         result = scene->CreateComponent_GameThread<Game::ComponentMetaType::DirectionalLight, DirectionalLightComponent>(*componentData);
      }

      delete componentData;

      return result;
   }

}

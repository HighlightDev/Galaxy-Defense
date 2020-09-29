#pragma once

#include "ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <glm/vec3.hpp>

namespace Game
{

	struct LightComponentData
      : public ComponentData
	{

      LightComponentData(const std::string& gameObjectName, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
      : ComponentData(gameObjectName)
      , Ambient(ambient)
      , Diffuse(diffuse)
      , Specular(specular) 
      , ShadowInfo(shadowInfo)
      {
      }

      glm::vec3 Ambient;
      glm::vec3 Diffuse;
      glm::vec3 Specular;
      ProjectedShadowInfo* ShadowInfo;
	};

}
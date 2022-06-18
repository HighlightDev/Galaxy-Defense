#pragma once

#include "LightComponentData.h"

namespace EngineCore
{

	struct DirectionalLightComponentData : 
      public LightComponentData
	{

      DirectionalLightComponentData(const std::string& gameObjectName, glm::vec3 rotation, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
         : LightComponentData(gameObjectName, ambient, diffuse, specular, shadowInfo, glm::vec3(), rotation, glm::vec3(1))
         , Direction(direction)
      {
      }

      glm::vec3 Direction;
	};

}
#pragma once

#include "Core/GameCore/Components/ComponentType.h"
#include "LightComponentData.h"

namespace Game
{

	struct DirectionalLightComponentData : 
      public LightComponentData
	{

      DirectionalLightComponentData(const std::string& gameObjectName, glm::vec3 rotation, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
         : LightComponentData(gameObjectName, ambient, diffuse, specular, shadowInfo)
         , Direction(direction)
         , Rotation(rotation)
      {
      }

      glm::vec3 Direction;
      glm::vec3 Rotation;
	};

}
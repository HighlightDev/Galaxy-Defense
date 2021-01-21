#pragma once

#include "Core/GameCore/Components/ComponentType.h"
#include "LightComponentData.h"

namespace Game
{

	struct PointLightComponentData : 
      public LightComponentData
	{

      PointLightComponentData(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 attenuation, float radianceSqrRadius, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
         : LightComponentData(gameObjectName, ambient, diffuse, specular, shadowInfo)
         , Attenuation(attenuation)
         , RadianceSqrRadius(radianceSqrRadius)
         , Translation(translation)
      {
      }

      glm::vec3 Attenuation;
      float RadianceSqrRadius;
      glm::vec3 Translation;
	};

}
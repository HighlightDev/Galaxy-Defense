#pragma once

#include "Core/GameCore/Components/ComponentType.h"
#include "LightComponentData.h"

namespace Game
{

	struct PointLightComponentData : 
      public LightComponentData
	{

      PointLightComponentData(glm::vec3 translation, glm::vec3 rotation, glm::vec3 attenuation, float radianceSqrRadius, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
         : LightComponentData(ambient, diffuse, specular, shadowInfo)
         , Attenuation(attenuation)
         , RadianceSqrRadius(radianceSqrRadius)
         , Translation(translation)
         , Rotation(rotation)
      {
      }

      virtual uint64_t GetType() const override {

         return POINT_LIGHT_COMPONENT;
      }

      glm::vec3 Attenuation;
      float RadianceSqrRadius;
      glm::vec3 Translation;
      glm::vec3 Rotation;
	};

}
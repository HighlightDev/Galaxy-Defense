#pragma once

#include "Core/GameCore/Components/ComponentType.h"
#include "LightComponentData.h"

namespace Game
{

	struct DirectionalLightComponentData : 
      public LightComponentData
	{

      DirectionalLightComponentData(glm::vec3 rotation, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
         : LightComponentData(ambient, diffuse, specular, shadowInfo)
         , Direction(direction)
         , Rotation(rotation)
      {
      }

      virtual uint64_t GetType() const override {

         return DIR_LIGHT_COMPONENT;
      }

      glm::vec3 Direction;
      glm::vec3 Rotation;
	};

}
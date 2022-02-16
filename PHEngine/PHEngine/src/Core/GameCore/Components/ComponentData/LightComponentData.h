#pragma once

#include "ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <glm/vec3.hpp>

namespace Game
{

	struct LightComponentData
      : public ComponentData
	{

      LightComponentData(const std::string& gameObjectName
      , glm::vec3 ambient
      , glm::vec3 diffuse
      , glm::vec3 specular
      , ProjectedShadowInfo* shadowInfo
      , glm::vec3 translation
      , glm::vec3 rotateEuelerAngles
      , glm::vec3 scale)
      : ComponentData(gameObjectName)
      , Ambient(ambient)
      , Diffuse(diffuse)
      , Specular(specular) 
      , ShadowInfo(shadowInfo)
      , Translation(translation)
      , Rotation(rotateEuelerAngles)
      , Scale(scale)
      {
      }

      glm::vec3 Ambient;
      glm::vec3 Diffuse;
      glm::vec3 Specular;
      ProjectedShadowInfo* ShadowInfo;
      glm::vec3 Translation;
      glm::vec3 Rotation;
      glm::vec3 Scale;
	};

}
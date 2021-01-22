#pragma once

#include "Core/GameCore/Components/ComponentType.h"
#include "PointLightComponentData.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"

namespace Game
{

   struct SpotlightComponentData :
      public PointLightComponentData
   {

      SpotlightComponentData(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 attenuation, float radianceSqrRadius, float cutoff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
         : PointLightComponentData(gameObjectName, translation, attenuation,radianceSqrRadius, ambient, diffuse, specular, shadowInfo)
         , Cutoff(cutoff)
         , Rotation(rotation)
      {
      }

      float Cutoff;
      glm::vec3 Rotation;
   };

}
#pragma once

#include "PointLightComponentData.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"

namespace EngineCore
{

   struct SpotlightComponentData :
      public PointLightComponentData
   {

      SpotlightComponentData(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 attenuation, float radianceRadius, float cutoff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
         : PointLightComponentData(gameObjectName, translation, attenuation, radianceRadius, ambient, diffuse, specular, shadowInfo)
         , Cutoff(cutoff)
      {
         Rotation = rotation;
      }

      float Cutoff;
   };

}
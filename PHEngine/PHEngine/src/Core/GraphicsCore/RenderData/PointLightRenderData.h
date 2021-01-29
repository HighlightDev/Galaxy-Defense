#pragma once

#include "LightRenderData.h"

namespace Graphics
{
   namespace Data
   {
      struct PointLightRenderData 
         : public LightRenderData
      {

         glm::vec3 Attenuation;

         float RadianceRadius;

         PointLightRenderData(glm::vec3 attenuation, float radianceRadius,
            glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
            : LightRenderData(ambient, diffuse, specular, shadowInfo)
            , Attenuation(attenuation)
            , RadianceRadius(radianceRadius)
         {
         }

         ~PointLightRenderData()
         {
         }
      };
   }
}

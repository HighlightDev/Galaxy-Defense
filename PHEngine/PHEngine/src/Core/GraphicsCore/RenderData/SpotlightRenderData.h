#pragma once

#include "PointLightRenderData.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"

namespace Graphics
{
   namespace Data
   {
      struct SpotlightRenderData
         : public PointLightRenderData
      {
         float Cutoff;

         SpotlightRenderData(glm::vec3 attenuation, float radianceSqrRadius, float cutoff,
            glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
            : PointLightRenderData(attenuation, radianceSqrRadius, ambient, diffuse, specular, shadowInfo)
            , Cutoff(cutoff)
         {
         }
      };
   }
}

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

         SpotlightRenderData(const glm::vec3& attenuation,
                             const float radianceSqrRadius,
                             const float cutoff,
                             const glm::vec3& ambient,
                             const glm::vec3& diffuse,
                             const glm::vec3& specular,
                             const std::shared_ptr<ProjectedShadowInfo> &shadowInfo)
             : PointLightRenderData(attenuation,
                                    radianceSqrRadius,
                                    ambient,
                                    diffuse,
                                    specular,
                                    shadowInfo),
               Cutoff(cutoff)
         {
         }
      };
   }
}

#pragma once

#include <glm/vec3.hpp>

namespace Graphics
{
   namespace Data
   {

      struct LightRenderData
      {

         glm::vec3 Ambient;
         glm::vec3 Diffuse;
         glm::vec3 Specular;
         ProjectedShadowInfo* ShadowInfo;

         LightRenderData(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, ProjectedShadowInfo* shadowInfo)
            : Ambient(ambient)
            , Diffuse(diffuse)
            , Specular(specular)
            , ShadowInfo(shadowInfo)
         {
         }

         ~LightRenderData()
         {
         }
      };
   }
}

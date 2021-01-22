#include "SpotlightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;

namespace Graphics
{
   namespace Proxy
   {

      SpotlightSceneProxy::SpotlightSceneProxy(const SpotlightComponent* component)
         : PointLightSceneProxy(component)
         , mCutoff(component->GetRenderData().Cutoff)
      {
      }

      SpotlightSceneProxy::~SpotlightSceneProxy()
      {
      }

      void SpotlightSceneProxy::PostLevelInit()
      {
         const auto shadowInfo = GetProjectedSpotLightShadowInfo();
         if (shadowInfo)
         {
            const float aspectRatio = shadowInfo->GetAtlasResource()->GetTextureAspectRatio();
            const auto shadowProjectionMatrix = glm::perspective<float>(DEG_TO_RAD(90.0f), aspectRatio, 1.0f, std::sqrtf(m_radianceSqrRadius));
          
            shadowInfo->SetShadowProjectionMatrix(shadowProjectionMatrix);
         }
      }

      LightSceneProxyType SpotlightSceneProxy::GetLightProxyType() const {

         return LightSceneProxyType::SPOT_LIGHT;
      }

      ProjectedSpotlightShadowInfo* SpotlightSceneProxy::GetProjectedSpotLightShadowInfo()
      {
         return static_cast<ProjectedSpotlightShadowInfo*>(GetShadowInfo());
      }

      glm::vec3 SpotlightSceneProxy::GetPosition() const
      {
         glm::vec3 result = m_relativeMatrix * glm::vec4(0, 0, 0, 1.0f);
         return result;
      }

      float SpotlightSceneProxy::GetCutoof() const
      {
         return mCutoff;
      }

      ProjectedShadowInfo* SpotlightSceneProxy::GetShadowInfo()
      {
         auto shadowInfo = static_cast<ProjectedSpotlightShadowInfo*>(m_shadowInfo);
         if (shadowInfo)
         {
            if (IsTransformationDirty())
            {
               //glm::vec3 lightPosition = GetPosition();
               //ProjectedPointLightShadowInfo::six_mat4x4 matrices;
               //matrices[0] = glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), -AXIS_UP);
               //matrices[1] = glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), -AXIS_UP);
               //matrices[2] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), AXIS_FORWARD);
               //matrices[3] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0), -AXIS_FORWARD);
               //matrices[4] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), -AXIS_UP);
               //matrices[5] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0), -AXIS_UP);

               //shadowInfo->SetShadowViewMatrices(matrices);
               //SetIsTransformationDirty(false);
            }
         }

         return shadowInfo;
      }

   }
}

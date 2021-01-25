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

      float SpotlightSceneProxy::GetCutoff() const
      {
         return mCutoff;
      }

      glm::vec3 SpotlightSceneProxy::GetTransformedDirectionVector(const glm::vec4& initialDirection) const
      {
         return m_relativeMatrix * initialDirection;
      }

      ProjectedShadowInfo* SpotlightSceneProxy::GetShadowInfo()
      {
         auto shadowInfo = static_cast<ProjectedSpotlightShadowInfo*>(m_shadowInfo);
         if (shadowInfo)
         {
            if (IsTransformationDirty())
            {
               const glm::vec4 lightDefaultDirection(1.0f, 0.0f, 0.0f, 0.0f);
               glm::vec3 direction = GetTransformedDirectionVector(lightDefaultDirection);
               direction = glm::normalize(direction);
               const glm::vec3& origin = GetPosition();

               const glm::mat4& shadowViewMatrix = glm::lookAt(origin, origin + direction, -AXIS_UP);
               shadowInfo->SetShadowViewMatrix(shadowViewMatrix);
               SetIsTransformationDirty(false);
            }
         }

         return shadowInfo;
      }

   }
}

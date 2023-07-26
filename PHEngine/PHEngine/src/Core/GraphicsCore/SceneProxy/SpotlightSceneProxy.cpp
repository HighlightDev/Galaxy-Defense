#include "SpotlightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;

namespace Graphics
{
   namespace Proxy
   {

      SpotlightSceneProxy::SpotlightSceneProxy(const SpotlightComponent *component)
          : PointLightSceneProxy(component),
            mCutoff(component->GetRenderData()->Cutoff)
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
            const auto shadowProjectionMatrix = glm::perspective<float>(DEG_TO_RAD(90.0f), aspectRatio, 1.0f, m_radianceRadius);

            shadowInfo->SetShadowProjectionMatrix(shadowProjectionMatrix);
         }
      }

      LightSceneProxyType SpotlightSceneProxy::GetLightProxyType() const
      {
         return LightSceneProxyType::SPOT_LIGHT;
      }

      std::shared_ptr<ProjectedSpotlightShadowInfo> SpotlightSceneProxy::GetProjectedSpotLightShadowInfo()
      {
         return std::static_pointer_cast<ProjectedSpotlightShadowInfo>(GetShadowInfo());
      }

      glm::vec3 SpotlightSceneProxy::GetDirection() const
      {
         static const glm::vec4 spotlightDefaultDirection{1.0f, 0.0f, 0.0f, 0.0f};
         glm::vec3 result = m_relativeMatrix * spotlightDefaultDirection;
         return result;
      }

      float SpotlightSceneProxy::GetCutoff() const
      {
         return mCutoff;
      }

      glm::vec3 SpotlightSceneProxy::GetTransformedDirectionVector(const glm::vec4 &initialDirection) const
      {
         return m_relativeMatrix * initialDirection;
      }

      std::shared_ptr<ProjectedShadowInfo> SpotlightSceneProxy::GetShadowInfo()
      {
         const auto &shadowInfo = std::static_pointer_cast<ProjectedSpotlightShadowInfo>(m_shadowInfo);
         if (shadowInfo)
         {
            if (IsTransformationDirty())
            {
               glm::vec3 direction = GetDirection();
               direction = glm::normalize(direction);
               const glm::vec3 &origin = GetPosition();

               const glm::mat4 &shadowViewMatrix = glm::lookAt(origin, origin + direction, -AXIS_UP);
               shadowInfo->SetShadowViewMatrix(shadowViewMatrix);
               SetIsTransformationDirty(false);
            }
         }

         return shadowInfo;
      }

   }
}

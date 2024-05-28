#include "PointLightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;

namespace Graphics
{
   namespace Proxy
   {

      PointLightSceneProxy::PointLightSceneProxy(const PointLightComponent *component)
          : LightSceneProxy(
                component->IsEnabled(),
                component->GetRelativeMatrix(),
                component->GetRenderData()->Ambient,
                component->GetRenderData()->Diffuse,
                component->GetRenderData()->Specular,
                component->GetRenderData()->ShadowInfo),
            m_attenuation(component->GetRenderData()->Attenuation),
            m_radianceRadius(component->GetRenderData()->RadianceRadius)
      {
      }

      PointLightSceneProxy::~PointLightSceneProxy()
      {
      }

      void PointLightSceneProxy::PostInitialize()
      {
         const auto shadowInfo = GetProjectedPointShadowInfo();
         if (shadowInfo)
         {
            const float aspectRatio = shadowInfo->GetAtlasResource()->GetTextureAspectRatio();
            const auto shadowProjectionMatrix = glm::perspective<float>(DEG_TO_RAD(90.0f), aspectRatio, 1.0f, m_radianceRadius);

            ProjectedPointLightShadowInfo::six_mat4x4 matrices;
            matrices[0] = shadowProjectionMatrix;
            matrices[1] = shadowProjectionMatrix;
            matrices[2] = shadowProjectionMatrix;
            matrices[3] = shadowProjectionMatrix;
            matrices[4] = shadowProjectionMatrix;
            matrices[5] = shadowProjectionMatrix;

            shadowInfo->SetShadowProjectionMatrix(matrices);
         }
      }

      LightSceneProxyType PointLightSceneProxy::GetLightProxyType() const
      {
         return LightSceneProxyType::POINT_LIGHT;
      }

      std::shared_ptr<ProjectedPointLightShadowInfo> PointLightSceneProxy::GetProjectedPointShadowInfo()
      {
         return std::static_pointer_cast<ProjectedPointLightShadowInfo>(GetShadowInfo());
      }

      glm::vec3 PointLightSceneProxy::GetPosition() const
      {
         glm::vec3 result = m_relativeMatrix * glm::vec4(0, 0, 0, 1.0f);
         return result;
      }

      glm::vec3 PointLightSceneProxy::GetAttenuation() const
      {
         return m_attenuation;
      }

      float PointLightSceneProxy::GetRadianceRadius() const
      {
         return m_radianceRadius;
      }

      std::shared_ptr<ProjectedShadowInfo> PointLightSceneProxy::GetShadowInfo()
      {
         const auto &shadowInfo = std::static_pointer_cast<ProjectedPointLightShadowInfo>(m_shadowInfo);
         if (shadowInfo)
         {
            if (IsTransformationDirty())
            {
               glm::vec3 lightPosition = GetPosition();
               ProjectedPointLightShadowInfo::six_mat4x4 matrices;
               matrices[0] = glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), -AXIS_UP);
               matrices[1] = glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), -AXIS_UP);
               matrices[2] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), AXIS_FORWARD);
               matrices[3] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0), -AXIS_FORWARD);
               matrices[4] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), -AXIS_UP);
               matrices[5] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0), -AXIS_UP);

               shadowInfo->SetShadowViewMatrices(matrices);
               SetIsTransformationDirty(false);
            }
         }

         return shadowInfo;
      }

   }
}

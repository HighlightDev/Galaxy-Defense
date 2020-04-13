#include "DirectionalLightSceneProxy.h" 

#include <glm/gtc/matrix_transform.hpp>

namespace Graphics
{
   namespace Proxy
   {

      DirectionalLightSceneProxy::DirectionalLightSceneProxy(const DirectionalLightComponent* component)
         : LightSceneProxy(
            component->GetRelativeMatrix(),
            component->GetRenderData().Ambient,
            component->GetRenderData().Diffuse,
            component->GetRenderData().Specular,
            component->GetRenderData().ShadowInfo)
         , m_direction(component->GetRenderData().Direction)
      {
      }

      DirectionalLightSceneProxy::~DirectionalLightSceneProxy()
      {

      }

      void DirectionalLightSceneProxy::PostConstructorInitialize()
      {
         auto shadowInfo = static_cast<ProjectedDirShadowInfo*>(m_shadowInfo);
         if (shadowInfo)
         {
            shadowInfo->SetShadowProjectionMatrix(glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f));
         }
      }

      ProjectedDirShadowInfo* DirectionalLightSceneProxy::GetProjectedDirShadowInfo()
      {
         auto result = static_cast<ProjectedDirShadowInfo*>(GetShadowInfo());
         return result;
      }

      LightSceneProxyType DirectionalLightSceneProxy::GetLightProxyType() const {

         return LightSceneProxyType::DIR_LIGHT;
      }

      ProjectedShadowInfo* DirectionalLightSceneProxy::GetShadowInfo()
      {
         auto shadowInfo = static_cast<ProjectedDirShadowInfo*>(m_shadowInfo);
         if (shadowInfo)
         {
            if (IsTransformationDirty())
            {
               // Build shadow view matrix
               glm::vec3 normLightDir = glm::normalize(GetDirection());

               // Target is the player, keep collecting shadow info when player is moving all around the level
               glm::vec3 targetPositon = shadowInfo->Offset;
               glm::vec3 lightTranslatedPosition = -(normLightDir * 50.0f);
               glm::vec3 shadowCastPosition(targetPositon + lightTranslatedPosition);

               shadowInfo->SetShadowViewMatrix(glm::lookAt(shadowCastPosition, targetPositon, glm::vec3(0, 1, 0)));

               SetIsTransformationDirty(false);
            }
         }

         return m_shadowInfo;
      }

   }
}

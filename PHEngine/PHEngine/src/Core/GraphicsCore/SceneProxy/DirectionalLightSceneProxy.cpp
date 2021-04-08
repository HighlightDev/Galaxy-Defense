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

      void DirectionalLightSceneProxy::PostLevelInit()
      {
         auto shadowInfo = static_cast<ProjectedDirectionalLightShadowInfo*>(m_shadowInfo);
         if (shadowInfo)
         {
            const float halfExtent = shadowInfo->GetShadowOrthoHalfExtent();
            shadowInfo->SetShadowProjectionMatrix(glm::ortho(-halfExtent, halfExtent, -halfExtent, halfExtent, 0.1f, halfExtent * 4));
         }
      }

      ProjectedDirectionalLightShadowInfo* DirectionalLightSceneProxy::GetProjectedDirShadowInfo()
      {
         auto result = static_cast<ProjectedDirectionalLightShadowInfo*>(GetShadowInfo());
         return result;
      }

      BoundingBox DirectionalLightSceneProxy::GetShadowOrthographicProjectionBound() const
      {
         BoundingBox orthoBox;

         auto shadowInfo = static_cast<ProjectedDirectionalLightShadowInfo*>(m_shadowInfo);
         if (shadowInfo)
         {
            // todo: make oriented bb or extend extent =/
            const float halfExtent = shadowInfo->GetShadowOrthoHalfExtent();
            glm::vec3 origin = shadowInfo->GetPlayerPositionOffset();
            orthoBox = BoundingBox(origin, glm::vec3(halfExtent * 1.5f, halfExtent * 1.5f, halfExtent * 1.5f));
         }

         return orthoBox;
      }

      LightSceneProxyType DirectionalLightSceneProxy::GetLightProxyType() const {

         return LightSceneProxyType::DIR_LIGHT;
      }

      ProjectedShadowInfo* DirectionalLightSceneProxy::GetShadowInfo()
      {
         auto shadowInfo = static_cast<ProjectedDirectionalLightShadowInfo*>(m_shadowInfo);
         if (shadowInfo)
         {
            if (IsTransformationDirty())
            {
               // Build shadow view matrix
               glm::vec3 normLightDir = glm::normalize(GetDirection());

               // Target is the player, keep collecting shadow info when player is moving all around the level
               glm::vec3 targetPositon = shadowInfo->GetPlayerPositionOffset();

               const float halfExtent = shadowInfo->GetShadowOrthoHalfExtent();

               glm::vec3 lightTranslatedPosition = -(normLightDir * (halfExtent * 2));
               glm::vec3 shadowCastPosition(targetPositon + lightTranslatedPosition);

               shadowInfo->SetShadowViewMatrix(glm::lookAt(shadowCastPosition, targetPositon, glm::vec3(0, 1, 0)));

               SetIsTransformationDirty(false);
            }
         }

         return m_shadowInfo;
      }

   }
}

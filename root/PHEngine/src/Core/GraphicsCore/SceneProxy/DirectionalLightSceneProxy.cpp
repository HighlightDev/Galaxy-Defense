#include "DirectionalLightSceneProxy.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Graphics {
namespace Proxy {

DirectionalLightSceneProxy::DirectionalLightSceneProxy(const DirectionalLightComponent* component)
    : LightSceneProxy(
        component->IsEnabled(),
        component->IsVisible(),
        component->GetWorldMatrix(),
        component->GetRenderData()->Ambient,
        component->GetRenderData()->Diffuse,
        component->GetRenderData()->Specular,
        component->GetRenderData()->ShadowInfo)
    , m_direction(component->GetRenderData()->Direction)
{
}

DirectionalLightSceneProxy::~DirectionalLightSceneProxy()
{
}

glm::vec3 DirectionalLightSceneProxy::GetDirection() const
{
    return m_worldMatrix * glm::vec4(m_direction, 0.0f);
}

void DirectionalLightSceneProxy::PostInitialize()
{
    const auto shadowInfoSp = std::static_pointer_cast<ProjectedDirectionalLightShadowInfo>(m_shadowInfo);
    if (shadowInfoSp && shadowInfoSp->GetAtlasResource()) {
        const float halfExtent = shadowInfoSp->GetShadowOrthoHalfExtent();
        shadowInfoSp->SetShadowProjectionMatrix(
            glm::ortho(-halfExtent, halfExtent, -halfExtent, halfExtent, 0.1f, halfExtent * 4));
    }
}

std::shared_ptr<ProjectedDirectionalLightShadowInfo> DirectionalLightSceneProxy::GetProjectedDirShadowInfo()
{
    return std::static_pointer_cast<ProjectedDirectionalLightShadowInfo>(GetShadowInfo());
}

BoundingBox3D DirectionalLightSceneProxy::GetShadowOrthographicProjectionBound() const
{
    BoundingBox3D orthoBox;

    const auto shadowInfoSp = std::static_pointer_cast<ProjectedDirectionalLightShadowInfo>(m_shadowInfo);
    if (shadowInfoSp) {
        const float halfExtent = shadowInfoSp->GetShadowOrthoHalfExtent();
        glm::vec3 origin = shadowInfoSp->GetPlayerPositionOffset();
        orthoBox = BoundingBox3D(origin, glm::vec3(halfExtent * 1.5f, halfExtent * 1.5f, halfExtent * 1.5f));
    }

    return orthoBox;
}

LightSceneProxyType DirectionalLightSceneProxy::GetLightProxyType() const
{
    return LightSceneProxyType::DIR_LIGHT;
}

std::shared_ptr<ProjectedShadowInfo> DirectionalLightSceneProxy::GetShadowInfo()
{
    const auto shadowInfoSp = std::static_pointer_cast<ProjectedDirectionalLightShadowInfo>(m_shadowInfo);
    if (shadowInfoSp) {
        if (IsTransformationDirty()) {
            // Build shadow view matrix
            glm::vec3 normLightDir = glm::normalize(GetDirection());

            // Target is the player, keep collecting shadow info when player is moving all around the level
            glm::vec3 targetPositon = shadowInfoSp->GetPlayerPositionOffset();

            const float halfExtent = shadowInfoSp->GetShadowOrthoHalfExtent();

            glm::vec3 lightTranslatedPosition = -(normLightDir * (halfExtent * 2));
            glm::vec3 shadowCastPosition(targetPositon + lightTranslatedPosition);

            shadowInfoSp->SetShadowViewMatrix(glm::lookAt(shadowCastPosition, targetPositon, glm::vec3(0, 1, 0)));

            mShadowCastPosition = shadowCastPosition;
            SetIsTransformationDirty(false);
        }
    }

    return m_shadowInfo;
}

} // namespace Proxy
} // namespace Graphics

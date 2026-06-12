#include "LightSceneProxy.h"

namespace Graphics {
namespace Proxy {

LightSceneProxy::LightSceneProxy(
    const bool isEnabled,
    const bool isVisible,
    const glm::mat4& worldMatrix,
    const glm::vec3& ambientColor,
    const glm::vec3& diffuseColor,
    const glm::vec3& specularColor,
    const std::shared_ptr<ProjectedShadowInfo>& shadowInfo)
    : SceneProxyBase(isEnabled)
    , m_worldMatrix(worldMatrix)
    , m_shadowInfo(shadowInfo)
    , mIsVisible(isVisible)
    , AmbientColor(ambientColor)
    , DiffuseColor(diffuseColor)
    , SpecularColor(specularColor)
{
    if (m_shadowInfo) {
        m_shadowInfo->SetOnShadowMapUpdatedCallback(std::bind(&LightSceneProxy::PostInitialize, this));
    }
}

LightSceneProxy::~LightSceneProxy()
{
}

void LightSceneProxy::CleanUp()
{
    if (m_shadowInfo) {
        m_shadowInfo->CleanUp();
    }
}

void LightSceneProxy::PostInitialize()
{
}

void LightSceneProxy::SetWorldMatrix(const glm::mat4& worldMatrix)
{
    m_worldMatrix = worldMatrix;
    SetIsTransformationDirty(true);
}

bool LightSceneProxy::IsTransformationDirty() const
{
    return bTransformationDirty;
}

void LightSceneProxy::SetIsTransformationDirty(bool value)
{
    bTransformationDirty = value;
}

void LightSceneProxy::SetIsVisible(const bool visible)
{
    mIsVisible = visible;
}

bool LightSceneProxy::IsVisible() const
{
    return mIsVisible;
}

std::shared_ptr<ProjectedShadowInfo> LightSceneProxy::GetShadowInfo()
{
    return m_shadowInfo;
}

glm::vec3 LightSceneProxy::GetShadowCastPosition() const
{
    return mShadowCastPosition;
}

} // namespace Proxy
} // namespace Graphics

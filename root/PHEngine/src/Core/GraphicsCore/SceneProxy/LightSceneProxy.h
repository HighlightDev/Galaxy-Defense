#pragma once

#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <memory>

using namespace Graphics;

namespace Graphics {
namespace Proxy {

enum class LightSceneProxyType { DIR_LIGHT, SPOT_LIGHT, POINT_LIGHT };

class LightSceneProxy : public SceneProxyBase {
protected:
    glm::mat4 m_worldMatrix;

    std::shared_ptr<ProjectedShadowInfo> m_shadowInfo;

    bool bTransformationDirty{true};

    bool mIsVisible{true};

    glm::vec3 mShadowCastPosition;

public:
    glm::vec3 AmbientColor;
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;

    LightSceneProxy(
        const bool isEnabled,
        const bool isVisible,
        const glm::mat4& worldMatrix,
        const glm::vec3& ambientColor,
        const glm::vec3& diffuseColor,
        const glm::vec3& specularColor,
        const std::shared_ptr<ProjectedShadowInfo>& shadowInfo);

    virtual ~LightSceneProxy();

    virtual LightSceneProxyType GetLightProxyType() const = 0;

    virtual void PostInitialize();

    void SetWorldMatrix(const glm::mat4& worldMatrix);

    bool IsTransformationDirty() const;

    void SetIsTransformationDirty(bool value);

    void SetIsVisible(const bool visible);

    bool IsVisible() const;

    virtual std::shared_ptr<ProjectedShadowInfo> GetShadowInfo();

    void CleanUp() override;

    glm::vec3 GetShadowCastPosition() const;
};

} // namespace Proxy
} // namespace Graphics

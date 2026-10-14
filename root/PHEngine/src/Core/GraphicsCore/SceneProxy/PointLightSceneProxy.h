#pragma once

#include "Core/GameCore/Components/PointLightComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "LightSceneProxy.h"

using namespace EngineCore;

namespace Graphics {
namespace Proxy {

class PointLightSceneProxy : public LightSceneProxy {
protected:
    glm::vec3 m_attenuation;

    float m_radianceRadius;

public:
    PointLightSceneProxy(const PointLightComponent* component);

    ~PointLightSceneProxy() override;

    void PostInitialize() override;

    glm::vec3 GetPosition() const;

    glm::vec3 GetAttenuation() const;

    float GetRadianceRadius() const;

    LightSceneProxyType GetLightProxyType() const override;

    std::shared_ptr<ProjectedPointLightShadowInfo> GetProjectedPointShadowInfo();

    std::shared_ptr<ProjectedShadowInfo> GetShadowInfo() override;
};

} // namespace Proxy
} // namespace Graphics

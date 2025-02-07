#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "LightSceneProxy.h"

using namespace EngineCore;

namespace Graphics {
namespace Proxy {

class DirectionalLightSceneProxy : public LightSceneProxy {
    glm::vec3 m_direction;

public:
    glm::vec3 GetDirection() const;

    DirectionalLightSceneProxy(const DirectionalLightComponent* component);

    ~DirectionalLightSceneProxy() override;

    void PostInitialize() override;

    std::shared_ptr<ProjectedDirectionalLightShadowInfo> GetProjectedDirShadowInfo();

    LightSceneProxyType GetLightProxyType() const override;

    std::shared_ptr<ProjectedShadowInfo> GetShadowInfo() override;

    BoundingBox3D GetShadowOrthographicProjectionBound() const;
};

} // namespace Proxy
} // namespace Graphics

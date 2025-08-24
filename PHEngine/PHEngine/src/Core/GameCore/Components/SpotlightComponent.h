#pragma once

#include "Core/GraphicsCore/RenderData/SpotlightRenderData.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "PointLightComponent.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;

namespace EngineCore {
struct LightComponentData;
class SpotlightComponent : public PointLightComponent {
    using Base = PointLightComponent;

public:
    SpotlightComponent(const std::shared_ptr<LightComponentData>& data);

    ~SpotlightComponent() override;

    void Tick(const float deltaTime) override;

    std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

    std::shared_ptr<SpotlightRenderData> GetRenderData() const;
};
} // namespace EngineCore

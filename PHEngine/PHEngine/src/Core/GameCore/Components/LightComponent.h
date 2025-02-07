#pragma once
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/RenderData/LightRenderData.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"

#include <glm/vec3.hpp>

#include <atomic>
#include <memory>

using namespace Graphics::Proxy;
using namespace Graphics::Data;

namespace EngineCore {
struct LightComponentData;

class LightComponent : public SceneComponent {
protected:
    using Base = SceneComponent;

    std::shared_ptr<LightRenderData> mLightRenderData;

    bool mIsVisible{false};

    std::atomic<bool> bIsSceneProxyReady{false};

    bool bIsEnabledStateDirty{false};

    bool bIsVisibleStateDirty{false};

    int32_t mLightSceneProxyId{-1};

public:
    LightComponent(const std::shared_ptr<LightComponentData>& data);

    virtual ~LightComponent();

    void SetIsSceneProxyReady(const bool isSceneProxyReady);

    bool IsSceneProxyReady() const;

    void SetLightSceneProxyId(const int32_t lightSceneProxyId);

    int32_t GetLightSceneProxyId() const;

    void UnpausableTick(const float deltaTime) override;

    void SetIsEnabled(const bool value) override;

    void SetIsVisible(const bool value);

    bool IsVisible() const;

    eComponentType GetComponentType() const override;

    void UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix) override;

    virtual std::shared_ptr<LightSceneProxy> CreateSceneProxy() const = 0;

    LightRenderData& GetLightRenderData();

    void SyncRenderData();
};

} // namespace EngineCore

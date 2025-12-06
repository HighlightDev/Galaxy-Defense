#pragma once

#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GraphicsCore/RenderData/DirectionalLightRenderData.h"
#include "LightComponent.h"

using namespace Graphics::Proxy;
using namespace Graphics::Data;
using namespace Event;

namespace EngineCore {
struct LightComponentData;

class DirectionalLightComponent : public LightComponent,
                                  public PlayerMovedGameThreadEvent,
                                  public PhysicsComponentUpdatedGameThreadEvent {

    using Base = LightComponent;

    glm::vec3 mPlayerTranslationOffset{};

    bool bIsRenderDataDirty{false};

public:
    DirectionalLightComponent(const std::shared_ptr<LightComponentData>& lightComponentData);

    ~DirectionalLightComponent() override;

    void OnRegistered() override;

    void Tick(const float deltaTimeSec) override;

    eComponentType GetComponentType() const override;

    std::shared_ptr<LightSceneProxy> CreateSceneProxy() const override;

    std::shared_ptr<DirectionalLightRenderData> GetRenderData() const;

    void UpdateWorldMatrix(const glm::mat4& parentWorldMatrix) override;

protected:
    void
    ProcessEvent(const PlayerMovedGameThreadEvent* sender, const typename PlayerMovedGameThreadEvent::EventData_t& data) override;
    void ProcessEvent(
        const PhysicsComponentUpdatedGameThreadEvent* sender,
        const typename PhysicsComponentUpdatedGameThreadEvent::EventData_t& data) override;

private:
    void SyncRenderData();
};

} // namespace EngineCore

#pragma once

#include "Core/GameCore/Actor.h"

using namespace EngineCore;

namespace EngineCore {
class StaticMeshComponent;
}

namespace Game {
enum class eSpaceStationActivityState { IDLE, ACTIVE };

class SpaceStationActor : public Actor {

    float mTimeSinceLastShoot{0.0f};

    bool mIsRayActive{false};

    eSpaceStationActivityState mSpacestationState{eSpaceStationActivityState::IDLE};

    float mShootRadius{0.0f};

    std::shared_ptr<::EngineCore::StaticMeshComponent> mRadiusMarkerComponent;

public:
    SpaceStationActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const float shootRadius);

    void Tick(const float deltaTime) override;

    bool CanShoot() const;

    void RestartTimerSinceLastShoot();

    void SetState(const eSpaceStationActivityState spacestationState);

    eSpaceStationActivityState GetState() const;

    void SetShootRadius(const float value);

    float GetShootRadius() const;

    void SetIsRayActive(const bool value);

    bool GetIsRayActive() const;

    void SetRadiusMarkerComponent(const std::shared_ptr<::EngineCore::StaticMeshComponent>& radiusMarkerComponent);
};
} // namespace Game

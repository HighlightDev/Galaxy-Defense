#pragma once

#include "Core/GameCore/Actor.h"
#include "Implementation/ActorLeveling/SpaceStationLevel.h"

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

    std::shared_ptr<::EngineCore::StaticMeshComponent> mRadiusMarkerComponent;

    std::shared_ptr<SpaceStationLevel> mSpaceStationLevel;

    std::shared_ptr<EngineObjectProperty<float>> mShootRadiusProperty;

public:
    SpaceStationActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTime) override;

    bool CanShoot() const;

    void RestartTimerSinceLastShoot();

    void SetState(const eSpaceStationActivityState spacestationState);

    eSpaceStationActivityState GetState() const;

    void SetSpaceStationLevel(const std::shared_ptr<SpaceStationLevel>& spaceStationLevel);

    const std::shared_ptr<SpaceStationLevel>& GetSpaceStationLevel() const;

    void SetIsRayActive(const bool value);

    bool GetIsRayActive() const;

    void SetRadiusMarkerComponent(const std::shared_ptr<::EngineCore::StaticMeshComponent>& radiusMarkerComponent);
};
} // namespace Game

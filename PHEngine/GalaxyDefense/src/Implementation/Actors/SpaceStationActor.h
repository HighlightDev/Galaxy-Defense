#pragma once

#include "Core/GameCore/Actor.h"

using namespace EngineCore;

namespace Game {
enum class eSpaceStationActivityState { IDLE, ACTIVE };

class SpaceStationActor : public Actor {

    float mTimeSinceLastShoot{0.0f};

    eSpaceStationActivityState mSpacestationState{eSpaceStationActivityState::IDLE};

public:
    SpaceStationActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTime) override;

    bool CanShoot() const;

    void RestartTimerSinceLastShoot();

    void SetState(const eSpaceStationActivityState spacestationState);

    eSpaceStationActivityState GetState() const;
};
} // namespace Game

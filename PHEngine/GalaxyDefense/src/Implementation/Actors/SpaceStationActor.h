#pragma once

#include "Core/GameCore/Actor.h"

using namespace EngineCore;

namespace Game {
class SpaceStationActor : public Actor {
    float mTimeSinceLastShoot{0.0f};

public:
    SpaceStationActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void Tick(const float deltaTime) override;

    bool CanShoot() const;

    void RestartTimerSinceLastShoot();
};
} // namespace Game

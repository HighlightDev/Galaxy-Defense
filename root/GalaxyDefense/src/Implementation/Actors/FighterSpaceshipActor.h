#pragma once

#include "Implementation/ActorLeveling/FighterSpaceshipLevel.h"
#include "WeakSpaceshipActor.h"

#include <memory>

using namespace EngineCore;

namespace Game {
class FighterSpaceshipActor : public WeakSpaceshipActor {

    FighterSpaceshipLevel mFighterLevel;

    float mShootCooldown{0.0f};

    bool mIsRayActive{false};

public:
    FighterSpaceshipActor(
        const std::string& gameObjectName,
        const std::shared_ptr<EngineCore::SceneComponent>& rootComponent,
        const FighterSpaceshipLevel& fighterLevel);

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void TriggerSpawn(const glm::vec3& position) override;

    void TriggerDisabled() override;

    bool CanShoot() const;

    void RestartTimerSinceLastShoot();

    void SetIsRayActive(const bool value);

    bool GetIsRayActive() const;

    const FighterSpaceshipLevel& GetFighterLevel() const;
};
} // namespace Game

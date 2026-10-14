#pragma once

#include "IModifiable.h"

#include <memory>

namespace Game {
class SpaceshipActor;
class MissileActor;

// Applied to a ship the instant a plasma bomb hits it: a self-contained burn that deals damage-over-time for a fixed
// duration (no pull, no deformation). Outlives the short-lived projectile, so its lifetime is its own timer rather than
// the missile's state.
class PlasmaModifier : public IModifiable {
    std::weak_ptr<SpaceshipActor> mOwnerWp;

    std::weak_ptr<MissileActor> mMissileWp;

    float mDamagePerSecond;

    float mDurationSec{3.0f};

    float mElapsedSec{0.0f};

    // Fractional HP accumulator: TriggerDamageReceived takes integer damage, so sub-1 HP per tick is carried over.
    float mDamageAccumulator{0.0f};

public:
    PlasmaModifier(const std::weak_ptr<SpaceshipActor>& owner, const std::weak_ptr<MissileActor>& missile);

    eModifierType GetModifierType() const override;

    int32_t CreatorObjectId() const override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override{};

    bool IsExpired() const override;

    void OnPreRemoved() override;

    void SetDamagePerSecond(const float damagePerSecond);

    void SetDurationSec(const float durationSec);
};
} // namespace Game

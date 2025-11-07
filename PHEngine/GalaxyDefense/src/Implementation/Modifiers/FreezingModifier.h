#pragma once

#include "IModifiable.h"

#include <glm/vec3.hpp>

#include <memory>

namespace Game {
class SpaceshipActor;

class FreezingModifier : public IModifiable {
    std::weak_ptr<SpaceshipActor> mOwnerWp;

    float mFreezingPower;

    float mFreezingTimer;
    float mFreezingTimeout;

public:
    FreezingModifier(const std::weak_ptr<SpaceshipActor>& owner);

    eModifierType GetModifierType() const override;

    int32_t CreatorObjectId() const override;

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override { };

    bool IsExpired() const override;

    void OnPreRemoved() override;

    void SetFreezingPower(const float power);

    void ResetFreezingTimer();

    void SetFreezingTimeout(const float timeout);

    float GetFreezingTimer() const;
};
} // namespace Game
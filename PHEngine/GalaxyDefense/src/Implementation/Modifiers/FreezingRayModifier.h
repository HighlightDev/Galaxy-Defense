#pragma once

#include "IModifiable.h"

#include <glm/vec3.hpp>

#include <memory>

namespace Game {
class SpaceshipActor;

class FreezingRayModifier : public IModifiable {
    std::weak_ptr<SpaceshipActor> mOwnerWp;

    float mFreezingPower;

    bool mIsExpired{false};

public:
    FreezingRayModifier(const std::weak_ptr<SpaceshipActor>& owner);

    eModifierType GetModifierType() const override;

    int32_t CreatorObjectId() const override;

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override { };

    void SetIsExpired(const bool isExpired);

    bool IsExpired() const override;

    void OnPreRemoved() override;

    void SetFreezingPower(const float power);
};
} // namespace Game
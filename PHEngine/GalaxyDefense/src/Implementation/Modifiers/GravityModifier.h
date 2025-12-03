#pragma once

#include "IModifiable.h"

#include <glm/vec3.hpp>

#include <memory>

namespace Game {
class SpaceshipActor;
class MissileActor;

class GravityModifier : public IModifiable {
    std::weak_ptr<SpaceshipActor> mOwnerWp;

    std::weak_ptr<MissileActor> mMissileWp;

    glm::vec3 mGravityCenterPosition;

    float mGravityPower;

public:
    GravityModifier(
        const std::weak_ptr<SpaceshipActor>& owner,
        const std::weak_ptr<MissileActor>& missile,
        const glm::vec3& gravityCenterPosition);

    eModifierType GetModifierType() const override;

    int32_t CreatorObjectId() const override;

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override{};

    bool IsExpired() const override;

    void OnPreRemoved() override;

    void SetGravityPower(const float power);
};
} // namespace Game
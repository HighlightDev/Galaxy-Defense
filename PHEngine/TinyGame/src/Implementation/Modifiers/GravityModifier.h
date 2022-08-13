#pragma once

#include "IModifiable.h"

#include <memory>
#include <glm/vec3.hpp>

namespace Game
{
    class SpaceshipActor;
    class MissileActor;

    class GravityModifiable : public IModifiable
    {
        std::weak_ptr<SpaceshipActor> mOwnerWp;

        std::weak_ptr<MissileActor> mMissileWp;

        glm::vec3 mGravityCenterPosition;

        float mGravityPower;

    public:
        GravityModifiable(const std::weak_ptr<SpaceshipActor> &owner,
                          const std::weak_ptr<MissileActor> &missile,
                          const glm::vec3 &gravityCenterPosition);

        virtual eModifierType GetModifierType() const override;

        virtual void Tick(const float deltaTime) override;

        void SetGravityPower(const float power);
    };
}
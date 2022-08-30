#pragma once

#include "IModifiable.h"

#include <memory>
#include <glm/vec3.hpp>

namespace Game
{
    class SpaceshipActor;

    class FreezingModifier : public IModifiable
    {
        std::weak_ptr<SpaceshipActor> mOwnerWp;

        float mFreezingPower;

        float mFreezingTimer;
        float mFreezingTimeout;

    public:
        FreezingModifier(const std::weak_ptr<SpaceshipActor> &owner);

        virtual eModifierType GetModifierType() const override;

        virtual uint64_t CreatorObjectId() const override;

        virtual void Tick(const float deltaTime) override;

        virtual bool IsExpired() const override;

        virtual void OnPreRemoved() override;

        void SetFreezingPower(const float power);

        void ResetFreezingTimer();

        void SetFreezingTimeout(const float timeout);

        float GetFreezingTimer() const;
    };
}
#pragma once

#include "Core/GameCore/ITickable.h"

#include <stdint.h>

namespace Game
{
    class SpaceshipActor;

    enum class eModifierType
    {
        Gravity,
        Freezing
    };

    class IModifiable : public ITickable
    {
    public:
        virtual eModifierType GetModifierType() const = 0;

        virtual uint64_t CreatorObjectId() const = 0;

        virtual bool IsExpired() const = 0;

        virtual void OnPreRemoved() = 0;
    };
}
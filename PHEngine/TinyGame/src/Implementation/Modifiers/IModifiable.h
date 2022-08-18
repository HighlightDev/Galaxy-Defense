#pragma once

#include "Core/GameCore/ITickable.h"

#include <stdint.h>

namespace Game
{
    class SpaceshipActor;

    enum class eModifierType
    {
        Gravity
    };

    class IModifiable : public ITickable
    {
    public:
        virtual eModifierType GetModifierType() const = 0;

        virtual uint64_t CreatorObjectId() const = 0;
    };
}
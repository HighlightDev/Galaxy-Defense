#pragma once

#include "Core/GameCore/ITickable.h"

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
    };
}
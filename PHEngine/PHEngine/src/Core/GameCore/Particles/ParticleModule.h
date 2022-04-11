#pragma once

#include "Core/GameCore/ITickable.h"

#include <memory>

namespace EngineCore
{
    class ParticleSystemComponent;

    class ParticleModule
        : public ITickable
    {
    protected:
        std::weak_ptr<ParticleSystemComponent> mOwner;

    public:
        ParticleModule(const std::weak_ptr<ParticleSystemComponent> &owner);

        virtual void Tick(const float deltaTime) = 0;
    };
}
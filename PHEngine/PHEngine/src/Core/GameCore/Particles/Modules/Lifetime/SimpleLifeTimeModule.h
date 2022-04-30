#pragma once

#include "Core/GameCore/Particles/Modules/Lifetime/ILifeTimeModule.h"

namespace EngineCore
{
    class SimpleLifeTimeModule
        : public ILifeTimeModule
    {
        float mLifeTime;

    public:
        SimpleLifeTimeModule();

        virtual void Update(Particle &particle, const float deltaTime) override;

        void SetLifeTime(const float lifeTime);

        virtual void OnEmitParticles() override;
    };
}
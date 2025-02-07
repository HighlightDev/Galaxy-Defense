#pragma once

#include "Core/GameCore/Particles/Modules/Lifetime/ILifeTimeModule.h"

namespace EngineCore {
class SimpleLifeTimeModule : public ILifeTimeModule {
    float mLifeTime;

public:
    SimpleLifeTimeModule();

    void Update(Particle& particle, const float deltaTime) override;

    void SetLifeTime(const float lifeTime);

    void OnEmitParticles() override;
};
} // namespace EngineCore
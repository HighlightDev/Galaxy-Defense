#pragma once

#include "Core/GameCore/Particles/Modules/Lifetime/ILifeTimeModule.h"

namespace EngineCore {
class SimpleLifeTimeModule : public ILifeTimeModule {
    float mLifeTime;

public:
    SimpleLifeTimeModule();

    void SetLifeTime(const float lifeTime);

    void OnEmitParticles() override;

    void EmitSingleParticle(Particle& particle) override;

    std::shared_ptr<IGpuParticleModuleProxy> GetGpuProxy() const override;
};
} // namespace EngineCore
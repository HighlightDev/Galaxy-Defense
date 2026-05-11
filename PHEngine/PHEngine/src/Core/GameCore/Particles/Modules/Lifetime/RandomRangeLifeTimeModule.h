#pragma once

#include "Core/GameCore/Particles/Modules/Lifetime/ILifeTimeModule.h"

namespace EngineCore {

// Assigns each particle a random lifetime uniformly distributed in [mMinLifeTime, mMaxLifeTime].
class RandomRangeLifeTimeModule : public ILifeTimeModule {
    float mMinLifeTime{0.3f};
    float mMaxLifeTime{1.0f};

public:
    RandomRangeLifeTimeModule();

    void OnEmitParticles() override;

    void EmitSingleParticle(Particle& particle) override;

    void SetMinLifeTime(const float minLifeTime);
    void SetMaxLifeTime(const float maxLifeTime);

    float GetMinLifeTime() const;
    float GetMaxLifeTime() const;

    std::shared_ptr<IGpuParticleModuleProxy> GetGpuProxy() const override;
};
} // namespace EngineCore

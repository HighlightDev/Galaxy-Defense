#pragma once

#include "Core/GameCore/Particles/Modules/IParticleModule.h"

namespace EngineCore {
class ILifeTimeModule : public IParticleModule {
public:
    ILifeTimeModule();

    eParticleModuleType GetParticleModuleType() const override;

    // Common update: decrements LifeRemaining and deactivates the particle when exhausted.
    void Update(Particle& particle, const float deltaTimeSec) override;
};
} // namespace EngineCore
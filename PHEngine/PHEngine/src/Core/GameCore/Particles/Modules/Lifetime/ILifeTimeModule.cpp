#include "ILifeTimeModule.h"

namespace EngineCore {
ILifeTimeModule::ILifeTimeModule()
{
}

eParticleModuleType ILifeTimeModule::GetParticleModuleType() const
{
    return eParticleModuleType::LIFETIME;
}

void ILifeTimeModule::Update(Particle& particle, const float deltaTimeSec)
{
    particle.LifeRemaining -= deltaTimeSec;
    if (particle.LifeRemaining <= 0.0f) {
        particle.isActive = false;
        particle.LifeRemaining = 0.0f;
    }
}
} // namespace EngineCore
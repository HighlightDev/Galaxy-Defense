#include "ExplosionInitialVelocityModule.h"

#include "Core/CommonCore/Random.h"

namespace EngineCore {
ExplosionInitialVelocityModule::ExplosionInitialVelocityModule()
{
}

eParticleModuleType ExplosionInitialVelocityModule::GetParticleModuleType() const
{
    return eParticleModuleType::INITIAL_VELOCITY;
}

void ExplosionInitialVelocityModule::Update(Particle& particle, const float deltaTime)
{
}

void ExplosionInitialVelocityModule::OnEmitParticles()
{
    auto& particlesPool = GetParticlesPool();
    const size_t particlesCount = particlesPool.size();

    for (size_t i = 0; i < particlesCount; ++i) {
        Particle& particle = particlesPool[i];

        const auto randomNormalizedValue = []() { return (Random::Float() * 2.0f) - 1.0f; };

        particle.InitialVelocity
            = glm::vec3(randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f);
    }
}
} // namespace EngineCore
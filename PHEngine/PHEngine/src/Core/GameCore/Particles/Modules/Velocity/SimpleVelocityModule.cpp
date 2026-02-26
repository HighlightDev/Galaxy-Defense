#include "SimpleVelocityModule.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/Velocity/SimpleVelocityModuleGpuProxy.h"

namespace EngineCore {
SimpleVelocityModule::SimpleVelocityModule()
    : mVelocityDirection()
    , mVelocityDeviation()
    , mCurrentSpawnVelocityDeviation()
{
}

void SimpleVelocityModule::Update(Particle& particle, const float deltaTimeSec)
{
    particle.Velocity
        = (mVelocityDirection * deltaTimeSec * mExtraVelocityPower) + (mCurrentSpawnVelocityDeviation * deltaTimeSec);
}

void SimpleVelocityModule::SetVelocityDirection(const glm::vec3& velocityDirection)
{
    mVelocityDirection = velocityDirection;
}

void SimpleVelocityModule::SetVelocityDeviation(const glm::vec3& velocityDeviation)
{
    mVelocityDeviation = velocityDeviation;
}

void SimpleVelocityModule::OnEmitParticles()
{
    const float signX = (Random::Float() * 2.0f) - 1.0f;
    const float signY = (Random::Float() * 2.0f) - 1.0f;
    const float signZ = (Random::Float() * 2.0f) - 1.0f;
    mCurrentSpawnVelocityDeviation = glm::vec3(signX, signY, signZ) * mVelocityDeviation;

    auto& particlesPool = GetParticlesPool();
    for (auto& particle : particlesPool) {
        const auto randomNormalizedValue = []() { return (Random::Float() * 2.0f) - 1.0f; };

        particle.InitialVelocity
            = glm::vec3(randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f);
    }
}

void SimpleVelocityModule::SetExtraVelocityPower(const float velocityPower)
{
    mExtraVelocityPower = velocityPower;
}

std::shared_ptr<IGpuParticleModuleProxy> SimpleVelocityModule::GetGpuProxy() const
{
    return std::make_shared<SimpleVelocityModuleGpuProxy>(mVelocityDirection, mVelocityDeviation, mExtraVelocityPower);
}
} // namespace EngineCore
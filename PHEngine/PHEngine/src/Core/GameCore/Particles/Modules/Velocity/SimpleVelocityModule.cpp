#include "SimpleVelocityModule.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/Velocity/SimpleVelocityModuleGpuProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/geometric.hpp>

namespace EngineCore {
SimpleVelocityModule::SimpleVelocityModule()
    : mVelocityDirection()
    , mVelocityDeviation()
    , mCurrentSpawnVelocityDeviation()
{
}

void SimpleVelocityModule::Update(Particle& particle, const float deltaTimeSec)
{
    // This module only sets initial velocity on emit, no need to update every frame.
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
        const auto randomNormalizedValue = []() {
            const auto value = (Random::Float() * 2.0f) - 1.0f;
            return EngineMath::FloatsNearEqual(value, 0.0f) ? 0.1f : value;
        };

        particle.InitialVelocity = glm::normalize(
            glm::vec3(randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f));
        particle.Velocity
            = glm::normalize(mVelocityDirection + mCurrentSpawnVelocityDeviation + particle.InitialVelocity) * mSpeed;
    }
}

void SimpleVelocityModule::EmitSingleParticle(Particle& particle)
{
    const auto randomNormalizedValue = []() { return (Random::Float() * 2.0f) - 1.0f; };
    const float signX = randomNormalizedValue();
    const float signY = randomNormalizedValue();
    const float signZ = randomNormalizedValue();

    mCurrentSpawnVelocityDeviation = glm::normalize(glm::vec3(signX, signY, signZ) * mVelocityDeviation);
    particle.InitialVelocity = glm::normalize(
        glm::vec3(randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f));
    particle.Velocity = glm::normalize(mVelocityDirection + mCurrentSpawnVelocityDeviation + particle.InitialVelocity) * mSpeed;
}

void SimpleVelocityModule::SetSpeed(const float speed)
{
    mSpeed = speed;
}

std::shared_ptr<IGpuParticleModuleProxy> SimpleVelocityModule::GetGpuProxy() const
{
    return std::make_shared<SimpleVelocityModuleGpuProxy>(mVelocityDirection, mVelocityDeviation, mSpeed);
}
} // namespace EngineCore
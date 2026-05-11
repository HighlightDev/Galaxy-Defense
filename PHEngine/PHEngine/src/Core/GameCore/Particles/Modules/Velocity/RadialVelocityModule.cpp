#include "RadialVelocityModule.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"
#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/Velocity/RadialVelocityModuleGpuProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/geometric.hpp>

namespace EngineCore {

RadialVelocityModule::RadialVelocityModule()
{
}

void RadialVelocityModule::ProcessEmitSingleParticle(const glm::vec3& particleCenter, Particle& particle)
{
    const auto randomNormalizedValue = []() { return (Random::Float() * 2.0f) - 1.0f; };
    const glm::vec3 randomVec
        = glm::vec3(randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f, randomNormalizedValue() * 2.0f);
    const float length = glm::length(randomVec);
    if (EngineMath::FloatsNearEqual(length, 0.0f)) {
        particle.InitialVelocity = glm::vec3(0.0f, 1.0f, 0.0f) * mSpeed; // default to upward if random vector is zero
    } else {
        particle.InitialVelocity = (randomVec / length) * mSpeed; // normalize and scale by speed
    }
    particle.Velocity = particle.InitialVelocity;
}

void RadialVelocityModule::OnEmitParticles()
{
    if (const auto ownerSp = mOwner.lock()) {
        const glm::vec3 center = ownerSp->GetHierarchyAccumulatedTranslation();
        auto& pool = GetParticlesPool();
        for (auto& particle : pool) {
            ProcessEmitSingleParticle(center, particle);
        }
    }
}

void RadialVelocityModule::Update([[maybe_unused]] Particle& particle, [[maybe_unused]] const float deltaTimeSec)
{
    // Radial velocity is constant, so no need to update it over time.
}

void RadialVelocityModule::EmitSingleParticle(Particle& particle)
{
    if (const auto ownerSp = mOwner.lock()) {
        const glm::vec3 center = ownerSp->GetHierarchyAccumulatedTranslation();
        ProcessEmitSingleParticle(center, particle);
    }
}

void RadialVelocityModule::SetSpeed(const float speed)
{
    mSpeed = speed;
}

float RadialVelocityModule::GetSpeed() const
{
    return mSpeed;
}

std::shared_ptr<IGpuParticleModuleProxy> RadialVelocityModule::GetGpuProxy() const
{
    return std::make_shared<RadialVelocityModuleGpuProxy>(mSpeed);
}

} // namespace EngineCore

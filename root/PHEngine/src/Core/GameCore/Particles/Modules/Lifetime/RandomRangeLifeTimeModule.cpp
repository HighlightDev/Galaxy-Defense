#include "RandomRangeLifeTimeModule.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/Lifetime/RandomRangeLifeTimeModuleGpuProxy.h"

namespace EngineCore {

RandomRangeLifeTimeModule::RandomRangeLifeTimeModule()
{
}

void RandomRangeLifeTimeModule::OnEmitParticles()
{
    auto& particles = GetParticlesPool();

    for (auto& particle : particles) {
        const float t = Random::Float(); // [0, 1]
        const float lifeTime = mMinLifeTime + t * (mMaxLifeTime - mMinLifeTime);
        particle.LifeTime = lifeTime;
        particle.LifeRemaining = lifeTime;
        particle.isActive = true;
    }
}

void RandomRangeLifeTimeModule::SetMinLifeTime(const float minLifeTime)
{
    mMinLifeTime = minLifeTime;
}

void RandomRangeLifeTimeModule::SetMaxLifeTime(const float maxLifeTime)
{
    mMaxLifeTime = maxLifeTime;
}

float RandomRangeLifeTimeModule::GetMinLifeTime() const
{
    return mMinLifeTime;
}

float RandomRangeLifeTimeModule::GetMaxLifeTime() const
{
    return mMaxLifeTime;
}

void RandomRangeLifeTimeModule::EmitSingleParticle(Particle& particle)
{
    const float lifeTime = mMinLifeTime + Random::Float() * (mMaxLifeTime - mMinLifeTime);
    particle.LifeTime = lifeTime;
    particle.LifeRemaining = lifeTime;
    particle.isActive = true;
}

std::shared_ptr<IGpuParticleModuleProxy> RandomRangeLifeTimeModule::GetGpuProxy() const
{
    return std::make_shared<RandomRangeLifeTimeModuleGpuProxy>(mMinLifeTime, mMaxLifeTime);
}

} // namespace EngineCore

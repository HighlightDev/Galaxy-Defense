#include "ParticleSystemBaseComponent.h"

#include "Core/GameCore/Particles/Emitters/IEmitter.h"

#include <algorithm>

namespace EngineCore {
ParticleSystemBaseComponent::ParticleSystemBaseComponent(
    const std::string& name,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    size_t particlesCount)
    : PrimitiveComponent(name, translation, rotation, scale)
    , mParticlesCount(particlesCount)
{
    mParticlesPool.resize(particlesCount);
    mSortOrderValue = std::numeric_limits<int32_t>::max(); // draw this primitive the last one
    mCanBloomBeApplied = true;
}

eComponentType ParticleSystemBaseComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

void ParticleSystemBaseComponent::AddParticleModule(const std::shared_ptr<IParticleModule>& particleModule)
{
    const auto& newModuleType = particleModule->GetParticleModuleType();
    auto foundSameModuleIt = std::find_if(mParticleModules.begin(), mParticleModules.end(), [=](const auto& particleModule) {
        return particleModule->GetParticleModuleType() == newModuleType;
    });
    ext_assert(
        foundSameModuleIt == mParticleModules.end(),
        "Particle module of the same type is already added to ParticleSystemBaseComponent");
    mParticleModules.emplace_back(particleModule);
    std::sort(mParticleModules.begin(), mParticleModules.end(), [](const auto& leftModule, const auto& rightModule) {
        return (uint8_t)leftModule->GetParticleModuleType() < (uint8_t)rightModule->GetParticleModuleType();
    });
}

void ParticleSystemBaseComponent::EmitParticles()
{
    mParticleEmitter->EmitParticles(mParticlesCount);

    for (const auto& particleModule : mParticleModules) {
        particleModule->OnEmitParticles();
    }
}

void ParticleSystemBaseComponent::ResetParticles()
{
    for (auto particleIt = mParticlesPool.begin(); particleIt != mParticlesPool.end(); ++particleIt) {
        particleIt->Reset();
    }

    if (bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        SyncDataWithRenderThread(0, true);
        mPrevActiveParticles = 0;
    }
}

size_t ParticleSystemBaseComponent::GetParticlesCount() const
{
    return mParticlesPool.size();
}

void ParticleSystemBaseComponent::SetParticleEmitter(const std::shared_ptr<IEmitter>& emitter)
{
    ext_assert(!mParticleEmitter, "Particle emitter is already set for ParticleSystemBaseComponent");
    mParticleEmitter = emitter;
}

const std::vector<Particle>& ParticleSystemBaseComponent::GetParticlesPool() const
{
    return mParticlesPool;
}
} // namespace EngineCore

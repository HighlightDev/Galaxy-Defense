#include "ParticleSystemBaseComponent.h"

#include "Core/GameCore/Particles/Emitters/IEmitter.h"

#include <algorithm>

#undef max
#undef min

namespace EngineCore {
ParticleSystemBaseComponent::ParticleSystemBaseComponent(
    const std::string& name,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const size_t particlesCount)
    : PrimitiveComponent(name, translation, rotation, scale)
    , mParticlesCount(particlesCount)
    , mPrevActiveParticles(0)
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

    SetIsParticleModulesProxiesDirty(true);
    if (bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        SyncDataWithRenderThread(0, true);
    }
}

void ParticleSystemBaseComponent::EmitParticles()
{
    mParticleEmitter->EmitParticles(mParticlesCount);

    for (const auto& particleModule : mParticleModules) {
        particleModule->OnEmitParticles();
    }

    bIsEmitting = true;
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

    bIsEmitting = false;
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

std::vector<std::shared_ptr<IParticleModule>> ParticleSystemBaseComponent::GetParticleModules() const
{
    return mParticleModules;
}

std::vector<std::shared_ptr<IGpuParticleModuleProxy>> ParticleSystemBaseComponent::GetParticleModulesProxies() const
{
    std::vector<std::shared_ptr<IGpuParticleModuleProxy>> gpuProxies;
    gpuProxies.reserve(mParticleModules.size());
    std::transform(
        mParticleModules.cbegin(),
        mParticleModules.cend(),
        std::back_inserter(gpuProxies),
        [](const std::shared_ptr<IParticleModule>& module) { return module->GetGpuProxy(); });
    return gpuProxies;
}

bool ParticleSystemBaseComponent::IsAnyParticleAlive() const
{
    return std::any_of(
        mParticlesPool.cbegin(), mParticlesPool.cend(), [](const Particle& particle) { return particle.isActive; });
}

void ParticleSystemBaseComponent::SetIsEndlessRespawnEnabled(const bool isEnabled)
{
    isEndlessRespawnEnabled = isEnabled;
}

bool ParticleSystemBaseComponent::IsEndlessRespawnEnabled() const
{
    return isEndlessRespawnEnabled;
}
} // namespace EngineCore

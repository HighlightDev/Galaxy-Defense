#pragma once

#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/Particles/Modules/IParticleModule.h"
#include "Core/GameCore/Particles/Particle.h"
#include "Core/GameCore/Particles/ParticleProperties.h"
#include "Core/GraphicsCore/RenderData/CpuParticleSystemRenderData.h"

using namespace Graphics::Data;

namespace EngineCore {
class IEmitter;

class ParticleSystemBaseComponent : public PrimitiveComponent {
    friend class IEmitter;
    friend class IParticleModule;

protected:
    size_t mParticlesCount;

    std::vector<Particle> mParticlesPool;

    size_t mPrevActiveParticles;

    std::vector<std::shared_ptr<IParticleModule>> mParticleModules;

    std::shared_ptr<IEmitter> mParticleEmitter;

public:
    ParticleSystemBaseComponent(
        const std::string& name,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        size_t particlesCount);

    virtual ~ParticleSystemBaseComponent() = default;

    eComponentType GetComponentType() const override;

    virtual void AddParticleModule(const std::shared_ptr<IParticleModule>& particleModule);

    virtual void EmitParticles();

    virtual void ResetParticles();

    size_t GetParticlesCount() const;

    void SetParticleEmitter(const std::shared_ptr<IEmitter>& emitter);

    const std::vector<Particle>& GetParticlesPool() const;

protected:
    virtual void SyncDataWithRenderThread(const size_t activeParticlesCount, const bool forceSyncData = false) = 0;
};
} // namespace EngineCore

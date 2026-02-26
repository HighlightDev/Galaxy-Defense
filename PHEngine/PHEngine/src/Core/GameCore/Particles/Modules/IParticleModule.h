#pragma once

#include "Core/GameCore/Particles/Particle.h"

#include <glm/vec4.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
enum class eParticleModuleType : uint8_t { LIFETIME, VELOCITY, COLOR, SIZE };

class ParticleSystemBaseComponent;
class IGpuParticleModuleProxy;

class IParticleModule {
protected:
    std::weak_ptr<ParticleSystemBaseComponent> mOwner;

public:
    IParticleModule();

    virtual eParticleModuleType GetParticleModuleType() const = 0;

    virtual void Update(Particle& particle, const float deltaTimeSec) = 0;

    virtual void OnEmitParticles() = 0;

    virtual std::shared_ptr<IGpuParticleModuleProxy> GetGpuProxy() const = 0;

    void SetOwner(const std::weak_ptr<ParticleSystemBaseComponent>& owner);

protected:
    std::vector<Particle>& GetParticlesPool();
};
} // namespace EngineCore
#pragma once

#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Particles/Particle.h"

#include <glm/vec4.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
class CpuParticleSystemComponent;

class IEmitter {
protected:
    std::weak_ptr<CpuParticleSystemComponent> mOwner;

public:
    IEmitter();

    void SetOwner(const std::weak_ptr<CpuParticleSystemComponent>& owner);

    virtual void EmitParticles(const size_t particlesCount) = 0;

protected:
    std::vector<Particle>& GetParticlesPool();
};

} // namespace EngineCore

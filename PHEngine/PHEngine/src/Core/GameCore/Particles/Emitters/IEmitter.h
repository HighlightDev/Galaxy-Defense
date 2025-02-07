#pragma once

#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/Particles/Particle.h"

#include <glm/vec4.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
class ParticleSystemComponent;

class IEmitter {
protected:
    std::weak_ptr<ParticleSystemComponent> mOwner;

public:
    IEmitter();

    void SetOwner(const std::weak_ptr<ParticleSystemComponent>& owner);

    virtual void EmitParticles(const size_t particlesCount) = 0;

protected:
    std::vector<Particle>& GetParticlesPool();
};

} // namespace EngineCore

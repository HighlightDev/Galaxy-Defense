#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Particles/Particle.h"

#include <vector>
#include <memory>
#include <glm/vec4.hpp>

namespace EngineCore
{
   enum class eParticleModuleType
   {
      INITIAL_VELOCITY,
      VELOCITY
   };

    class ParticleSystemComponent;

    class IParticleModule
        : public ITickable
    {
    protected:
        std::weak_ptr<ParticleSystemComponent> mOwner;

    public:
       IParticleModule();

       virtual eParticleModuleType GetParticleModuleType() const = 0;

        virtual void Tick(const float deltaTime) = 0;

        virtual void OnEmitParticles() = 0;

        void SetOwner(const std::weak_ptr<ParticleSystemComponent> &owner);

    protected:

       std::vector<Particle>& GetParticlesPool();
    };
}
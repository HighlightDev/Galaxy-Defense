#pragma once

#include "Core/GameCore/Particles/Modules/Velocity/IVelocityModule.h"

namespace EngineCore
{
   class ExplosionInitialVelocityModule
      : public IVelocityModule
   {
   public:
      ExplosionInitialVelocityModule();

      virtual eParticleModuleType GetParticleModuleType() const override;

      virtual void Update(Particle& particle, const float deltaTime) override;

      virtual void OnEmitParticles() override;

   };
}
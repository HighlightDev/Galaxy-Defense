#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Particles/Modules/IVelocityModule.h"

namespace EngineCore
{
   class ExplosionInitialVelocityModule
      : public IVelocityModule
   {
   public:
      ExplosionInitialVelocityModule();

      virtual eParticleModuleType GetParticleModuleType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void OnEmitParticles() override;

   };
}
#pragma once

#include "IEmitter.h"

namespace EngineCore
{
   class ParticleExplosionEmitter 
      : public IEmitter
   {
      virtual void EmitParticles(const size_t particlesCount = 0) override;
   };

}


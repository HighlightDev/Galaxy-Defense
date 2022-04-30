#include "IVelocityModule.h"

namespace EngineCore
{
   IVelocityModule::IVelocityModule()
    {
    }

   eParticleModuleType IVelocityModule::GetParticleModuleType() const 
   {
      return eParticleModuleType::VELOCITY;
   }
}
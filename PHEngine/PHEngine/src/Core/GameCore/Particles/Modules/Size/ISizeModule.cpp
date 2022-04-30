#include "ISizeModule.h"

namespace EngineCore
{
   ISizeModule::ISizeModule()
   {
   }

   eParticleModuleType ISizeModule::GetParticleModuleType() const
   {
      return eParticleModuleType::SIZE;
   }
}
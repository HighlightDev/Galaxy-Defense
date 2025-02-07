#include "ILifeTimeModule.h"

namespace EngineCore {
ILifeTimeModule::ILifeTimeModule()
{
}

eParticleModuleType ILifeTimeModule::GetParticleModuleType() const
{
    return eParticleModuleType::LIFETIME;
}
} // namespace EngineCore
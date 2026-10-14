#include "IColorModule.h"

namespace EngineCore {
IColorModule::IColorModule()
{
}

eParticleModuleType IColorModule::GetParticleModuleType() const
{
    return eParticleModuleType::COLOR;
}
} // namespace EngineCore
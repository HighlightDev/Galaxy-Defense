#pragma once

#include "Core/GameCore/Particles/Modules/IParticleModule.h"

namespace EngineCore {
class IVelocityModule : public IParticleModule {
public:
    IVelocityModule();

    eParticleModuleType GetParticleModuleType() const override;
};
} // namespace EngineCore
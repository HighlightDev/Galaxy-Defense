#pragma once

#include "Core/GameCore/Particles/Modules/IParticleModule.h"

namespace EngineCore {
class IColorModule : public IParticleModule {
public:
    IColorModule();

    eParticleModuleType GetParticleModuleType() const override;
};
} // namespace EngineCore
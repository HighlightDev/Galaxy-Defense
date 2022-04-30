#pragma once

#include "Core/GameCore/Particles/Modules/IParticleModule.h"

namespace EngineCore
{
    class IVelocityModule
        : public IParticleModule
    {
    public:
       IVelocityModule();

       virtual eParticleModuleType GetParticleModuleType() const override;
    };
}
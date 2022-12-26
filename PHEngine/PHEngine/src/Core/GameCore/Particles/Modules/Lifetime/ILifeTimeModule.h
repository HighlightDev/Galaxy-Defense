#pragma once

#include "Core/GameCore/Particles/Modules/IParticleModule.h"

namespace EngineCore
{
    class ILifeTimeModule
        : public IParticleModule
    {
    public:
       ILifeTimeModule();

       eParticleModuleType GetParticleModuleType() const override;
    };
}
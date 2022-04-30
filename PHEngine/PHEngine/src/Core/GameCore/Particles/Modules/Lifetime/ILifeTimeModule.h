#pragma once

#include "Core/GameCore/Particles/Modules/IParticleModule.h"

namespace EngineCore
{
    class ILifeTimeModule
        : public IParticleModule
    {
    public:
       ILifeTimeModule();

       virtual eParticleModuleType GetParticleModuleType() const override;
    };
}
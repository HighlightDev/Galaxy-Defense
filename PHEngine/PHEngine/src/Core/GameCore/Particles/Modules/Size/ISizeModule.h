#pragma once

#include "Core/GameCore/Particles/Modules/IParticleModule.h"

namespace EngineCore
{
    class ISizeModule
        : public IParticleModule
    {
    public:
       ISizeModule();

       eParticleModuleType GetParticleModuleType() const override;
    };
}
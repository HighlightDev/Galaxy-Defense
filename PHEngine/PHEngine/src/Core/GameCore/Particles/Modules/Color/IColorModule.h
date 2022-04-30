#pragma once

#include "Core/GameCore/Particles/Modules/IParticleModule.h"

namespace EngineCore
{
    class IColorModule
        : public IParticleModule
    {
    public:
        IColorModule();

        virtual eParticleModuleType GetParticleModuleType() const override;
    };
}
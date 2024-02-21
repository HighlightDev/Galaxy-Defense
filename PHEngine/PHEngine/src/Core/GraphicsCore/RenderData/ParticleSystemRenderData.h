#pragma once

#include <memory>

#include "Core/GraphicsCore/Material/MaterialProxy.h"
#include "Core/GameCore/Particles/ParticlePoolParameters.h"

using namespace EngineCore;

namespace Graphics
{
    namespace Data
    {
        struct ParticleSystemRenderData
        {
            ParticlePoolParameters mParticleMeshParams;
            std::shared_ptr<MaterialProxy> mMaterialProxy;

            ParticleSystemRenderData(
                const ParticlePoolParameters &particleMeshParams,
                std::shared_ptr<MaterialProxy> materialProxy)
                : mParticleMeshParams(particleMeshParams),
                  mMaterialProxy(materialProxy)
            {
            }
        };
    }
}

#pragma once

#include "Core/GraphicsCore/Material/MaterialProxy.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/SSBOPoolParameters.h"

#include <memory>

using namespace Resources;
using namespace EngineCore;

namespace Graphics {
namespace Data {
struct GpuParticleSystemRenderData {
    ParticlePoolParameters mParticleMeshParams;
    SSBOPoolParameters mSSBOPoolParams;
    std::shared_ptr<MaterialProxy> mMaterialProxy;

    GpuParticleSystemRenderData(
        const ParticlePoolParameters& particleMeshParams,
        const SSBOPoolParameters& ssboPoolParams,
        std::shared_ptr<MaterialProxy> materialProxy)
        : mParticleMeshParams(particleMeshParams)
        , mSSBOPoolParams(ssboPoolParams)
        , mMaterialProxy(materialProxy)
    {
    }
};
} // namespace Data
} // namespace Graphics

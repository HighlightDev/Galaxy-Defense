#pragma once

#include "Core/GraphicsCore/Material/MaterialProxy.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/ParticlePoolParameters.h"

#include <memory>

using namespace Resources;
using namespace EngineCore;

namespace Graphics {
namespace Data {
struct ParticleSystemRenderData {
    ParticlePoolParameters mParticleMeshParams;
    std::shared_ptr<MaterialProxy> mMaterialProxy;

    ParticleSystemRenderData(const ParticlePoolParameters& particleMeshParams, std::shared_ptr<MaterialProxy> materialProxy)
        : mParticleMeshParams(particleMeshParams)
        , mMaterialProxy(materialProxy)
    {
    }
};
} // namespace Data
} // namespace Graphics

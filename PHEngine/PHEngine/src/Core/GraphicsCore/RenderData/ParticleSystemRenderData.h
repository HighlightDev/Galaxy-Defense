#pragma once

#include <memory>

#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"
#include "Core/GameCore/Particles/ParticlePoolParameters.h"

using namespace Graphics::OpenGL;
using namespace Graphics::Texture;
using namespace EngineCore;

namespace Graphics
{
    namespace Data
    {
        struct ParticleSystemRenderData
        {
            ParticlePoolParameters mParticleMeshParams;
            std::shared_ptr<IShader> m_shader;
            std::shared_ptr<MaterialProxy> mMaterialProxy;

            ParticleSystemRenderData(
                const ParticlePoolParameters &particleMeshParams,
                std::shared_ptr<IShader> particleShader,
                std::shared_ptr<MaterialProxy> materialProxy)
                : mParticleMeshParams(particleMeshParams),
                  m_shader(particleShader),
                  mMaterialProxy(materialProxy)
            {
            }
        };
    }
}

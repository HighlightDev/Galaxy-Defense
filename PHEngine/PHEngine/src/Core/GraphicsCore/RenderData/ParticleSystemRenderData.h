#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"

using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
    namespace Data
    {

        struct ParticleSystemRenderData
        {
            std::shared_ptr<Skin> m_skin;
            std::shared_ptr<IShader> m_shader;
            std::shared_ptr<MaterialProxy> mMaterialProxy;

            ParticleSystemRenderData(std::shared_ptr<Skin> particleMesh,
                                     std::shared_ptr<IShader> particleShader,
                                     std::shared_ptr<MaterialProxy> materialProxy)
                : m_skin(particleMesh),
                  m_shader(particleShader),
                  mMaterialProxy(materialProxy)
            {
            }
        };
    }
}

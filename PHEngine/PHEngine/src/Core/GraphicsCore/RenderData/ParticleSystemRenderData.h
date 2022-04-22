#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"

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

            ParticleSystemRenderData(std::shared_ptr<Skin> particleMesh,
                                     std::shared_ptr<IShader> particleShader)
                : m_skin(particleMesh),
                  m_shader(particleShader)
            {
            }
        };
    }
}

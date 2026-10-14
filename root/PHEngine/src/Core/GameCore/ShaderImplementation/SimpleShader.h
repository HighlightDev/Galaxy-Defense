#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {
class SimpleShader : public Shader {

    using Base = Shader;

public:
    SimpleShader(const ShaderParams& params)
        : Shader(params)
    {
    }

    void AccessAllUniformLocations(uint32_t shaderProgramId)
    {
        Base::AccessAllUniformLocations(shaderProgramId);
    }
};

} // namespace ShaderImpl
} // namespace EngineCore

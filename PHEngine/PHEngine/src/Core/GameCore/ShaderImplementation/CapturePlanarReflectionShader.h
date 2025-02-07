#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <cstddef>
#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {
class CapturePlanarReflectionShader : public Shader {
    using Base = Shader;

protected:
    Uniform uClipPlane;

public:
    CapturePlanarReflectionShader(const ShaderParams& params);

    ~CapturePlanarReflectionShader() override;

    void SetClipPlane(const glm::vec4& plane);

    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};
} // namespace ShaderImpl
} // namespace EngineCore
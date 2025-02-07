#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {
class ResolveFxColorShader : public Shader {
private:
    Uniform u_sceneColorTexture;
    Uniform u_bloomColorTexture;

public:
    ResolveFxColorShader(const ShaderParams& params);

    ~ResolveFxColorShader() override;

    void SetSceneColorTexture(const int32_t textureSlot);

    void SetBloomColorTexture(const int32_t textureSlot);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramID) override;

    void SetShaderPredefine() override;
};

} // namespace ShaderImpl
} // namespace EngineCore

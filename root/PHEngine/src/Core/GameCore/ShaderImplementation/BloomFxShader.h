#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {
class BloomFxShader : public Shader {
private:
    Uniform u_sceneColorTexture;
    Uniform u_bluredColorTexture;

    uint32_t mExtractBrightPartsSubroutineIndex;
    uint32_t mVerticalBlurSubroutineIndex;
    uint32_t mHorizontalBlurSubroutineIndex;
    uint32_t mResolveBloomColorSubroutineIndex;

public:
    BloomFxShader(const ShaderParams& params);

    ~BloomFxShader() override;

    void SetSceneColorTexture(const int32_t textureSlot);

    void SetBluredColorTexture(const int32_t textureSlot);

    void LoadExtractBrightPartsSubroutine();

    void LoadRunVerticalBlurSubroutine();

    void LoadRunHorizontalBlurSubroutine();

    void LoadRunResolveBloomColorSubroutine();

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramID) override;

    void AccessAllSubroutineIndices(uint32_t shaderProgramID) override;

    void SetShaderPredefine() override;
};

} // namespace ShaderImpl
} // namespace EngineCore

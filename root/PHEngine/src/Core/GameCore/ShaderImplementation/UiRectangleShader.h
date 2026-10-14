#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {
class UiRectangleShader : public Shader {
    using Base = Shader;

    Uniform u_color;
    Uniform u_opacity;
    Uniform u_translation;
    Uniform u_scale;
    Uniform u_widthHeightPx;
    Uniform u_borderRadius;
    Uniform u_isRoundTop;
    Uniform u_isRoundBottom;
    Uniform u_blurSampler;
    Uniform u_applyBlur;
    Uniform u_blurMix;
    Uniform u_screenResolution;

public:
    UiRectangleShader(const ShaderParams& params);

    void SetColor(const glm::vec3& color);

    void SetOpacity(const float opacity);

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetBorderRadius(const float borderRadiusPx);

    void SetWidthHeightPixels(const glm::vec2& widthHeightPixels);

    void SetIsRoundTop(const bool bIsRoundTop);

    void SetIsRoundBottom(const bool bIsRoundBottom);

    void SetBlurSampler(const int32_t textureSlot);

    void SetApplyBlur(const bool applyBlur);

    void SetBlurMix(const float blurMix);

    void SetScreenResolution(const glm::vec2& screenResolution);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace ShaderImpl
} // namespace EngineCore

#include "UiRectangleShader.h"

namespace EngineCore {
namespace ShaderImpl {

UiRectangleShader::UiRectangleShader(const ShaderParams& params)
    : Shader(params)
{
    Base::ShaderInit();
}

void UiRectangleShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Base::AccessAllUniformLocations(shaderProgramId);

    u_color = GetUniform("color", shaderProgramId);
    u_translation = GetUniform("translation", shaderProgramId);
    u_scale = GetUniform("scale", shaderProgramId);
    u_opacity = GetUniform("opacity", shaderProgramId);
    u_widthHeightPx = GetUniform("widthAndHeight", shaderProgramId);
    u_borderRadius = GetUniform("borderRadius", shaderProgramId);
    u_isRoundTop = GetUniform("isRoundTop", shaderProgramId);
    u_isRoundBottom = GetUniform("isRoundBottom", shaderProgramId);
    u_blurSampler = GetUniform("blurSampler", shaderProgramId);
    u_applyBlur = GetUniform("applyBlur", shaderProgramId);
    u_blurMix = GetUniform("blurMix", shaderProgramId);
    u_screenResolution = GetUniform("screenResolution", shaderProgramId);
}

void UiRectangleShader::SetColor(const glm::vec3& color)
{
    u_color.LoadUniform(color);
}

void UiRectangleShader::SetOpacity(const float opacity)
{
    u_opacity.LoadUniform(opacity);
}

void UiRectangleShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
{
    u_translation.LoadUniform(normalizedTranslation);
    u_scale.LoadUniform(normalizedScale);
}

void UiRectangleShader::SetBorderRadius(const float borderRadiusPx)
{
    u_borderRadius.LoadUniform(borderRadiusPx);
}

void UiRectangleShader::SetWidthHeightPixels(const glm::vec2& widthHeightPixels)
{
    u_widthHeightPx.LoadUniform(widthHeightPixels);
}

void UiRectangleShader::SetIsRoundTop(const bool bIsRoundTop)
{
    u_isRoundTop.LoadUniform(bIsRoundTop);
}

void UiRectangleShader::SetIsRoundBottom(const bool bIsRoundBottom)
{
    u_isRoundBottom.LoadUniform(bIsRoundBottom);
}

void UiRectangleShader::SetBlurSampler(const int32_t textureSlot)
{
    u_blurSampler.LoadUniform(textureSlot);
}

void UiRectangleShader::SetApplyBlur(const bool applyBlur)
{
    u_applyBlur.LoadUniform(applyBlur);
}

void UiRectangleShader::SetBlurMix(const float blurMix)
{
    u_blurMix.LoadUniform(blurMix);
}

void UiRectangleShader::SetScreenResolution(const glm::vec2& screenResolution)
{
    u_screenResolution.LoadUniform(screenResolution);
}

void UiRectangleShader::SetShaderPredefine()
{
}

} // namespace ShaderImpl
} // namespace EngineCore

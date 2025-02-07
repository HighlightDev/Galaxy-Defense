#include "UiProgressBarShader.h"

namespace EngineCore {
namespace ShaderImpl {

UiProgressBarShader::UiProgressBarShader(const ShaderParams& params)
    : Shader(params)
{
    Base::ShaderInit();
}

void UiProgressBarShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Base::AccessAllUniformLocations(shaderProgramId);

    u_emptyColor = GetUniform("emptyColor", shaderProgramId);
    u_filledColor = GetUniform("filledColor", shaderProgramId);
    u_fillPercentValue = GetUniform("fillPercentValue", shaderProgramId);
    u_translation = GetUniform("translation", shaderProgramId);
    u_scale = GetUniform("scale", shaderProgramId);
    u_opacity = GetUniform("opacity", shaderProgramId);
    u_widthHeightPx = GetUniform("widthAndHeight", shaderProgramId);
    u_borderRadius = GetUniform("borderRadius", shaderProgramId);
}

void UiProgressBarShader::SetEmptyColor(const glm::vec3& color)
{
    u_emptyColor.LoadUniform(color);
}

void UiProgressBarShader::SetFilledColor(const glm::vec3& color)
{
    u_filledColor.LoadUniform(color);
}

void UiProgressBarShader::SetFillPercentValue(const float fillPercentValue)
{
    u_fillPercentValue.LoadUniform(fillPercentValue);
}

void UiProgressBarShader::SetOpacity(const float opacity)
{
    u_opacity.LoadUniform(opacity);
}

void UiProgressBarShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
{
    u_translation.LoadUniform(normalizedTranslation);
    u_scale.LoadUniform(normalizedScale);
}

void UiProgressBarShader::SetBorderRadius(const float borderRadiusPx)
{
    u_borderRadius.LoadUniform(borderRadiusPx);
}

void UiProgressBarShader::SetWidthHeightPixels(const glm::vec2& widthHeightPixels)
{
    u_widthHeightPx.LoadUniform(widthHeightPixels);
}

void UiProgressBarShader::SetShaderPredefine()
{
}

} // namespace ShaderImpl
} // namespace EngineCore

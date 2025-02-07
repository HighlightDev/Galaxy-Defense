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

void UiRectangleShader::SetShaderPredefine()
{
}

} // namespace ShaderImpl
} // namespace EngineCore

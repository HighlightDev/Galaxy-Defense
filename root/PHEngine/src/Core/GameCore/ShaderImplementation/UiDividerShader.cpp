#include "UiDividerShader.h"

namespace EngineCore::ShaderImpl {

UiDividerShader::UiDividerShader(const ShaderParams& params)
    : Shader(params)
{
    Base::ShaderInit();
}

void UiDividerShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Base::AccessAllUniformLocations(shaderProgramId);

    u_color = GetUniform("color", shaderProgramId);
    u_translation = GetUniform("translation", shaderProgramId);
    u_scale = GetUniform("scale", shaderProgramId);
    u_opacity = GetUniform("opacity", shaderProgramId);
    u_widthHeightPx = GetUniform("widthAndHeight", shaderProgramId);
    u_lineWidthPx = GetUniform("lineWidthPx", shaderProgramId);
    u_edgeFade = GetUniform("edgeFade", shaderProgramId);
    u_orientation = GetUniform("orientation", shaderProgramId);
    u_screenResolution = GetUniform("screenResolution", shaderProgramId);
}

void UiDividerShader::SetColor(const glm::vec3& color)
{
    u_color.LoadUniform(color);
}

void UiDividerShader::SetOpacity(const float opacity)
{
    u_opacity.LoadUniform(opacity);
}

void UiDividerShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
{
    u_translation.LoadUniform(normalizedTranslation);
    u_scale.LoadUniform(normalizedScale);
}

void UiDividerShader::SetLineWidthPx(const float lineWidthPx)
{
    u_lineWidthPx.LoadUniform(lineWidthPx);
}

void UiDividerShader::SetEdgeFade(const float edgeFade)
{
    u_edgeFade.LoadUniform(edgeFade);
}

void UiDividerShader::SetOrientation(const int32_t orientation)
{
    u_orientation.LoadUniform(orientation);
}

void UiDividerShader::SetWidthHeightPixels(const glm::vec2& widthHeightPixels)
{
    u_widthHeightPx.LoadUniform(widthHeightPixels);
}

void UiDividerShader::SetScreenResolution(const glm::vec2& screenResolution)
{
    u_screenResolution.LoadUniform(screenResolution);
}

void UiDividerShader::SetShaderPredefine()
{
}

} // namespace EngineCore::ShaderImpl

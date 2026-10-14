#include "UiConnectionLineShader.h"

namespace Game {

UiConnectionLineShader::UiConnectionLineShader(const ShaderParams& params)
    : Shader(params)
{
    Base::ShaderInit();
}

void UiConnectionLineShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Base::AccessAllUniformLocations(shaderProgramId);

    u_translation = GetUniform("translation", shaderProgramId);
    u_scale = GetUniform("scale", shaderProgramId);
    u_widthAndHeight = GetUniform("widthAndHeight", shaderProgramId);
    u_startPoint = GetUniform("startPoint", shaderProgramId);
    u_endPoint = GetUniform("endPoint", shaderProgramId);
    u_color = GetUniform("color", shaderProgramId);
    u_thicknessPx = GetUniform("thicknessPx", shaderProgramId);
    u_dashLengthPx = GetUniform("dashLengthPx", shaderProgramId);
    u_gapLengthPx = GetUniform("gapLengthPx", shaderProgramId);
    u_opacity = GetUniform("opacity", shaderProgramId);
}

void UiConnectionLineShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
{
    u_translation.LoadUniform(normalizedTranslation);
    u_scale.LoadUniform(normalizedScale);
}

void UiConnectionLineShader::SetWidthHeightPixels(const glm::vec2& widthHeightPixels)
{
    u_widthAndHeight.LoadUniform(widthHeightPixels);
}

void UiConnectionLineShader::SetStartPoint(const glm::vec2& startPoint)
{
    u_startPoint.LoadUniform(startPoint);
}

void UiConnectionLineShader::SetEndPoint(const glm::vec2& endPoint)
{
    u_endPoint.LoadUniform(endPoint);
}

void UiConnectionLineShader::SetColor(const glm::vec3& color)
{
    u_color.LoadUniform(color);
}

void UiConnectionLineShader::SetThicknessPx(const float thicknessPx)
{
    u_thicknessPx.LoadUniform(thicknessPx);
}

void UiConnectionLineShader::SetDashLengthPx(const float dashLengthPx)
{
    u_dashLengthPx.LoadUniform(dashLengthPx);
}

void UiConnectionLineShader::SetGapLengthPx(const float gapLengthPx)
{
    u_gapLengthPx.LoadUniform(gapLengthPx);
}

void UiConnectionLineShader::SetOpacity(const float opacity)
{
    u_opacity.LoadUniform(opacity);
}

void UiConnectionLineShader::SetShaderPredefine()
{
}

} // namespace Game

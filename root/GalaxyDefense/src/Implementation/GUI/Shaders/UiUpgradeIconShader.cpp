#include "UiUpgradeIconShader.h"

namespace Game {

UiUpgradeIconShader::UiUpgradeIconShader(const ShaderParams& params)
    : Shader(params)
{
    Base::ShaderInit();
}

void UiUpgradeIconShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Base::AccessAllUniformLocations(shaderProgramId);

    u_image = GetUniform("image", shaderProgramId);
    u_translation = GetUniform("translation", shaderProgramId);
    u_scale = GetUniform("scale", shaderProgramId);
    u_rotationRadians = GetUniform("rotationRadians", shaderProgramId);
    u_isFlipped = GetUniform("isFlipped", shaderProgramId);
    u_widthAndHeight = GetUniform("widthAndHeight", shaderProgramId);
    u_fillColor = GetUniform("fillColor", shaderProgramId);
    u_borderColor = GetUniform("borderColor", shaderProgramId);
    u_iconCustomColor = GetUniform("iconCustomColor", shaderProgramId);
    u_isCustomIconColor = GetUniform("isCustomIconColor", shaderProgramId);
    u_fillStrength = GetUniform("fillStrength", shaderProgramId);
    u_borderThicknessPx = GetUniform("borderThicknessPx", shaderProgramId);
    u_opacity = GetUniform("opacity", shaderProgramId);
}

void UiUpgradeIconShader::SetImageTexture(const int32_t texSlot)
{
    u_image.LoadUniform(texSlot);
}

void UiUpgradeIconShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
{
    u_translation.LoadUniform(normalizedTranslation);
    u_scale.LoadUniform(normalizedScale);
}

void UiUpgradeIconShader::SetRotationRadians(const float rotationRadians)
{
    u_rotationRadians.LoadUniform(rotationRadians);
}

void UiUpgradeIconShader::SetIsFlipped(const bool isFlipped)
{
    u_isFlipped.LoadUniform(static_cast<int32_t>(isFlipped));
}

void UiUpgradeIconShader::SetWidthHeightPixels(const glm::vec2& widthHeightPixels)
{
    u_widthAndHeight.LoadUniform(widthHeightPixels);
}

void UiUpgradeIconShader::SetFillColor(const glm::vec3& color)
{
    u_fillColor.LoadUniform(color);
}

void UiUpgradeIconShader::SetBorderColor(const glm::vec3& color)
{
    u_borderColor.LoadUniform(color);
}

void UiUpgradeIconShader::SetIconCustomColor(const glm::vec3& color)
{
    u_iconCustomColor.LoadUniform(color);
}

void UiUpgradeIconShader::SetIsCustomIconColor(const bool isCustomIconColor)
{
    u_isCustomIconColor.LoadUniform(isCustomIconColor ? 1.0f : 0.0f);
}

void UiUpgradeIconShader::SetFillStrength(const float fillStrength)
{
    u_fillStrength.LoadUniform(fillStrength);
}

void UiUpgradeIconShader::SetBorderThicknessPx(const float borderThicknessPx)
{
    u_borderThicknessPx.LoadUniform(borderThicknessPx);
}

void UiUpgradeIconShader::SetOpacity(const float opacity)
{
    u_opacity.LoadUniform(opacity);
}

void UiUpgradeIconShader::SetShaderPredefine()
{
}

} // namespace Game

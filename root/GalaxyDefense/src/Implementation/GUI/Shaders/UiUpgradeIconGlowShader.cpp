#include "UiUpgradeIconGlowShader.h"

namespace Game {

UiUpgradeIconGlowShader::UiUpgradeIconGlowShader(const ShaderParams& params)
    : Shader(params)
{
    Base::ShaderInit();
}

void UiUpgradeIconGlowShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Base::AccessAllUniformLocations(shaderProgramId);

    u_translation = GetUniform("translation", shaderProgramId);
    u_scale = GetUniform("scale", shaderProgramId);
    u_rotationRadians = GetUniform("rotationRadians", shaderProgramId);
    u_isFlipped = GetUniform("isFlipped", shaderProgramId);
    u_widthAndHeight = GetUniform("widthAndHeight", shaderProgramId);
    u_iconWidthAndHeight = GetUniform("iconWidthAndHeight", shaderProgramId);
    u_glowColor = GetUniform("glowColor", shaderProgramId);
    u_glowSizePx = GetUniform("glowSizePx", shaderProgramId);
    u_opacity = GetUniform("opacity", shaderProgramId);
}

void UiUpgradeIconGlowShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
{
    u_translation.LoadUniform(normalizedTranslation);
    u_scale.LoadUniform(normalizedScale);
    u_rotationRadians.LoadUniform(0.0f);
    u_isFlipped.LoadUniform(static_cast<int32_t>(0));
}

void UiUpgradeIconGlowShader::SetWidthHeightPixels(const glm::vec2& widthHeightPixels)
{
    u_widthAndHeight.LoadUniform(widthHeightPixels);
}

void UiUpgradeIconGlowShader::SetIconWidthHeightPixels(const glm::vec2& iconWidthHeightPixels)
{
    u_iconWidthAndHeight.LoadUniform(iconWidthHeightPixels);
}

void UiUpgradeIconGlowShader::SetGlowColor(const glm::vec3& color)
{
    u_glowColor.LoadUniform(color);
}

void UiUpgradeIconGlowShader::SetGlowSizePx(const float glowSizePx)
{
    u_glowSizePx.LoadUniform(glowSizePx);
}

void UiUpgradeIconGlowShader::SetOpacity(const float opacity)
{
    u_opacity.LoadUniform(opacity);
}

void UiUpgradeIconGlowShader::SetShaderPredefine()
{
}

} // namespace Game

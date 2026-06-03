#include "FontRenderingShader.h"

namespace EngineCore {
namespace ShaderImpl {
FontRenderingShader::FontRenderingShader(const ShaderParams& params)
    : Shader(params)
{
    ShaderInit();
}

FontRenderingShader::~FontRenderingShader()
{
}

void FontRenderingShader::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    Shader::AccessAllUniformLocations(shaderProgramID);

    u_fontAtlas = GetUniform("fontAtlas", shaderProgramID);
    u_position = GetUniform("position", shaderProgramID);
    u_shadowWidth = GetUniform("shadowWidth", shaderProgramID);
    u_shadowOffset = GetUniform("shadowOffset", shaderProgramID);
    u_opacity = GetUniform("opacity", shaderProgramID);
}

void FontRenderingShader::SetFontAtlasSlot(const int32_t slot)
{
    u_fontAtlas.LoadUniform(slot);
}

void FontRenderingShader::SetPosition(const glm::vec2& position)
{
    u_position.LoadUniform(position);
}

void FontRenderingShader::SetShadowWidth(const float shadowWidth)
{
    u_shadowWidth.LoadUniform(shadowWidth);
}

void FontRenderingShader::SetShadowOffset(const glm::vec2& shadowOffset)
{
    u_shadowOffset.LoadUniform(shadowOffset);
}

void FontRenderingShader::SetOpacity(const float opacity)
{
    u_opacity.LoadUniform(opacity);
}

void FontRenderingShader::SetShaderPredefine()
{
}
} // namespace ShaderImpl
} // namespace EngineCore

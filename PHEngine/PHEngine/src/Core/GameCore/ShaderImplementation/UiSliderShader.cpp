#include "UiSliderShader.h"

namespace EngineCore::ShaderImpl {

UiSliderShader::UiSliderShader(const ShaderParams& params)
    : Shader(params)
{
    Base::ShaderInit();
}

void UiSliderShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Base::AccessAllUniformLocations(shaderProgramId);

    u_translation = GetUniform("translation", shaderProgramId);
    u_scale = GetUniform("scale", shaderProgramId);
    u_opacity = GetUniform("opacity", shaderProgramId);
    u_color = GetUniform("color", shaderProgramId);
    u_widthHeightPx = GetUniform("widthAndHeight", shaderProgramId);
    u_borderRadius = GetUniform("borderRadius", shaderProgramId);

    mRenderSliderLineSubroutineIndex
        = GetSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), "renderSliderLine");
    mRenderSliderBlobSubroutineIndex
        = GetSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), "renderSliderBlob");
}

void UiSliderShader::SetOpacity(const float opacity)
{
    u_opacity.LoadUniform(opacity);
}

void UiSliderShader::SetColor(const glm::vec3& color)
{
    u_color.LoadUniform(color);
}

void UiSliderShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
{
    u_translation.LoadUniform(normalizedTranslation);
    u_scale.LoadUniform(normalizedScale);
}

void UiSliderShader::SetBorderRadius(const float borderRadiusPx)
{
    u_borderRadius.LoadUniform(borderRadiusPx);
}

void UiSliderShader::SetWidthHeightPixels(const glm::vec2& widthHeightPixels)
{
    u_widthHeightPx.LoadUniform(widthHeightPixels);
}

void UiSliderShader::SetShaderPredefine()
{
}

void UiSliderShader::LoadRenderSliderLineSubroutine()
{
    LoadSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), 1, &mRenderSliderLineSubroutineIndex);
}

void UiSliderShader::LoadRenderSliderBlobSubroutine()
{
    LoadSubroutineIndex(MapShaderTypeToOpenGLConstant(eShaderType::FragmentShader), 1, &mRenderSliderBlobSubroutineIndex);
}

} // namespace EngineCore::ShaderImpl
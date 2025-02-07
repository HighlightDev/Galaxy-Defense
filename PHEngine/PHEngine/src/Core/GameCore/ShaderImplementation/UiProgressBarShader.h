#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {
class UiProgressBarShader : public Shader {
    using Base = Shader;

    Uniform u_emptyColor;
    Uniform u_filledColor;
    Uniform u_fillPercentValue;
    Uniform u_opacity;
    Uniform u_translation;
    Uniform u_scale;
    Uniform u_widthHeightPx;
    Uniform u_borderRadius;

public:
    UiProgressBarShader(const ShaderParams& params);

    void SetEmptyColor(const glm::vec3& color);

    void SetFilledColor(const glm::vec3& color);

    void SetFillPercentValue(const float fillPercentValue);

    void SetOpacity(const float opacity);

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetBorderRadius(const float borderRadiusPx);

    void SetWidthHeightPixels(const glm::vec2& widthHeightPixels);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace ShaderImpl
} // namespace EngineCore

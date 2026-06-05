#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore::ShaderImpl {
class UiDividerShader : public Shader {
    using Base = Shader;

    Uniform u_color;
    Uniform u_opacity;
    Uniform u_translation;
    Uniform u_scale;
    Uniform u_widthHeightPx;
    Uniform u_lineWidthPx;
    Uniform u_edgeFade;
    Uniform u_orientation;
    Uniform u_screenResolution;

public:
    UiDividerShader(const ShaderParams& params);

    void SetColor(const glm::vec3& color);

    void SetOpacity(const float opacity);

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetLineWidthPx(const float lineWidthPx);

    void SetEdgeFade(const float edgeFade);

    void SetOrientation(const int32_t orientation);

    void SetWidthHeightPixels(const glm::vec2& widthHeightPixels);

    void SetScreenResolution(const glm::vec2& screenResolution);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace EngineCore::ShaderImpl

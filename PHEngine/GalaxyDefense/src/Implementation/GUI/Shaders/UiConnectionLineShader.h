#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace Graphics::OpenGL;

namespace Game {

class UiConnectionLineShader : public Shader {
    using Base = Shader;

    Uniform u_translation;
    Uniform u_scale;
    Uniform u_widthAndHeight;
    Uniform u_startPoint;
    Uniform u_endPoint;
    Uniform u_color;
    Uniform u_thicknessPx;
    Uniform u_dashLengthPx;
    Uniform u_gapLengthPx;
    Uniform u_opacity;

public:
    UiConnectionLineShader(const ShaderParams& params);

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetWidthHeightPixels(const glm::vec2& widthHeightPixels);

    void SetStartPoint(const glm::vec2& startPoint);

    void SetEndPoint(const glm::vec2& endPoint);

    void SetColor(const glm::vec3& color);

    void SetThicknessPx(const float thicknessPx);

    void SetDashLengthPx(const float dashLengthPx);

    void SetGapLengthPx(const float gapLengthPx);

    void SetOpacity(const float opacity);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace Game

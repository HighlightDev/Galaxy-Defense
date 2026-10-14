#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace Graphics::OpenGL;

namespace Game {

class UiUpgradeIconShader : public Shader {
    using Base = Shader;

    Uniform u_image;
    Uniform u_translation;
    Uniform u_scale;
    Uniform u_rotationRadians;
    Uniform u_isFlipped;
    Uniform u_widthAndHeight;
    Uniform u_fillColor;
    Uniform u_borderColor;
    Uniform u_iconCustomColor;
    Uniform u_isCustomIconColor;
    Uniform u_fillStrength;
    Uniform u_borderThicknessPx;
    Uniform u_opacity;

public:
    UiUpgradeIconShader(const ShaderParams& params);

    void SetImageTexture(const int32_t texSlot);

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetRotationRadians(const float rotationRadians);

    void SetIsFlipped(const bool isFlipped);

    void SetWidthHeightPixels(const glm::vec2& widthHeightPixels);

    void SetFillColor(const glm::vec3& color);

    void SetBorderColor(const glm::vec3& color);

    void SetIconCustomColor(const glm::vec3& color);

    void SetIsCustomIconColor(const bool isCustomIconColor);

    void SetFillStrength(const float fillStrength);

    void SetBorderThicknessPx(const float borderThicknessPx);

    void SetOpacity(const float opacity);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace Game

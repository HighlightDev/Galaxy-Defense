#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace Graphics::OpenGL;

namespace Game {

class UiUpgradeIconGlowShader : public Shader {
    using Base = Shader;

    Uniform u_translation;
    Uniform u_scale;
    Uniform u_rotationRadians;
    Uniform u_isFlipped;
    Uniform u_widthAndHeight;
    Uniform u_iconWidthAndHeight;
    Uniform u_glowColor;
    Uniform u_glowSizePx;
    Uniform u_opacity;

public:
    UiUpgradeIconGlowShader(const ShaderParams& params);

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetWidthHeightPixels(const glm::vec2& widthHeightPixels);

    void SetIconWidthHeightPixels(const glm::vec2& iconWidthHeightPixels);

    void SetGlowColor(const glm::vec3& color);

    void SetGlowSizePx(const float glowSizePx);

    void SetOpacity(const float opacity);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace Game

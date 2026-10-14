#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore::ShaderImpl {
class UiSliderShader : public Shader {
    using Base = Shader;

    Uniform u_opacity;
    Uniform u_color;
    Uniform u_translation;
    Uniform u_scale;
    Uniform u_widthHeightPx;
    Uniform u_borderRadius;

    uint32_t mRenderSliderLineSubroutineIndex;
    uint32_t mRenderSliderBlobSubroutineIndex;

public:
    UiSliderShader(const ShaderParams& params);

    void SetOpacity(const float opacity);

    void SetColor(const glm::vec3& color);

    void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

    void SetBorderRadius(const float borderRadiusPx);

    void SetWidthHeightPixels(const glm::vec2& widthHeightPixels);

    void LoadRenderSliderLineSubroutine();

    void LoadRenderSliderBlobSubroutine();

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace EngineCore::ShaderImpl
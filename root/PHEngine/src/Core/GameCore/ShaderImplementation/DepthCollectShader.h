#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {

class DepthCollectShader : public Shader {
    Uniform u_lightWorldPosition;
    Uniform u_invShadowDistance;
    Uniform u_bWriteDepthLinearly;

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

public:
    DepthCollectShader(const ShaderParams& params);

    void SetLightWorldPosition(const glm::vec3& position);

    void SetInvShadowDistance(const float invShadowDistance);

    void SetWriteDepthLinearly(const bool value);

    void SetShaderPredefine() override;
};

} // namespace ShaderImpl
} // namespace EngineCore
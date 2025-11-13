#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

using namespace Graphics::OpenGL;

namespace EngineCore {
namespace ShaderImpl {

class BillboardShader : public Shader {

private:
    Uniform u_billboardExtent, u_screenResolution, u_applyScreenAspectRatio, u_rotationRadians;

public:
    BillboardShader(const ShaderParams& params);

    ~BillboardShader() override;

    void SetExtent(const float extent);

    void SetScreenResolution(const glm::vec2& screenResolution);

    void SetApplyScreenAspectRatio(const bool applyScreenAspectRatio);

    void SetRotationRadians(const float rotationRadians);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramID) override;
};

} // namespace ShaderImpl
} // namespace EngineCore

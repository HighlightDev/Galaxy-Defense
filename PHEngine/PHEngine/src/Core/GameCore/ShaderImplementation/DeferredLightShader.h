#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "Core/GraphicsCore/OpenGL/Shader/UniformBuffer.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"
#include "UniformBufferStructure/DeferredLightShaderUniformBufferStructure.h"

#include <string>

using namespace Graphics::OpenGL;
using namespace Graphics::Proxy;
using namespace EngineCore::ShaderImpl::UniformBufferStructure;

namespace EngineCore {
namespace ShaderImpl {

class DeferredLightShader : public Shader {

    using Base = Shader;

    Uniform u_CameraWorldPosition;

    DeferredLightShaderUniformBufferStructure<5, 20> mLightData;

    std::shared_ptr<UniformBuffer> u_dataBuffer;

#ifndef NO_LIT
    UniformArray u_DirectionalLightShadowMaps;
    UniformArray u_PointLightShadowMaps;

    UniformArray u_SpotlightAmbientColor;
    UniformArray u_SpotlightDiffuseColor;
    UniformArray u_SpotlightSpecularColor;
    UniformArray u_SpotlightDirection;
    UniformArray u_SpotlightPosition;
    UniformArray u_SpotlightCutoff;

    UniformArray u_SpotlightShadowMaps;
    UniformArray u_SpotlightShadowProjectionFarPlane;
    UniformArray u_SpotlightShadowMatrices;
    UniformArray u_SpotlightAtlasOffset;
    Uniform u_SpotlightShadowMapCount;
    Uniform u_SpotlightCount;
#endif

    Uniform u_gBuffer_Position;
    Uniform u_gBuffer_Normal;
    Uniform u_gBuffer_Albedo;
    Uniform u_gBuffer_Emission;

#ifdef SHADING_MODEL_PBR
    Uniform u_gBuffer_MetallicRoughness;
#endif

public:
    DeferredLightShader(const ShaderParams& params);

    ~DeferredLightShader() override;

    void SetGBufferAlbedo(int32_t slot);

    void SetGBufferNormal(int32_t slot);

    void SetGBufferPosition(int32_t slot);

#ifdef SHADING_MODEL_PBR
    void SetGBufferMetallicRoughness(int32_t slot);
#endif

    void SetGBufferEmission(const int32_t slot);

#ifndef NO_LIT
    void SetLightsInfo(const std::vector<std::shared_ptr<LightSceneProxy>>& lightsProxies);

    void SetDirectionalLightShadowMapSlot(size_t index, int32_t slot);
    void SetDirectionalLightShadowMapSlot(size_t index, int32_t slot, const glm::vec4& atlasOffset);
    void SetDirectionalLightShadowMapCount(int32_t count);
    void SetDirectionalLightShadowMatrix(size_t index, const glm::mat4& shadowMatrix);

    void SetPointLightShadowMapSlot(size_t index, int32_t slot);
    void SetPointLightShadowMapCount(int32_t count);
    void SetPointLightShadowProjectionFarPlane(size_t index, float FarPlane);

    void SetSpotlightShadowMapSlot(size_t index, int32_t slot, const glm::vec4& atlasOffset);
    void SetSpotlightShadowMapCount(int32_t count);
    void SetSpotlightShadowProjectionFarPlane(size_t index, float FarPlane);
    void SetSpotlightShadowMatrix(size_t index, const glm::mat4& shadowMatrix);
#endif

    void SetCameraWorldPosition(const glm::vec3& cameraWorldPosition);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace ShaderImpl
} // namespace EngineCore

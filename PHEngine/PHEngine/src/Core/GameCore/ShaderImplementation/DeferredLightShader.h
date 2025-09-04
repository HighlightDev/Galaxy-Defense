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

    DeferredLightShaderUniformBufferStructure<5, 20, 20> mLightData;

    std::shared_ptr<UniformBuffer> u_dataBuffer;

#ifndef NO_LIT
    UniformArray u_DirectionalLightShadowMaps;
    UniformArray u_PointLightShadowMaps;
    UniformArray u_SpotlightShadowMaps;
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

    void SetDirectionalLightShadowMapSlot(size_t index, int32_t slot, const glm::vec4& atlasOffset);
    void SetPointLightShadowMapSlot(size_t index, int32_t slot);
    void SetSpotlightShadowMapSlot(size_t index, int32_t slot, const glm::vec4& atlasOffset);
#endif

    void SetCameraWorldPosition(const glm::vec3& cameraWorldPosition);

protected:
    void AccessAllUniformLocations(uint32_t shaderProgramId) override;

    void SetShaderPredefine() override;
};

} // namespace ShaderImpl
} // namespace EngineCore

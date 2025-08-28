#include "DeferredLightShader.h"

#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/ResourceManagerCore/Pool/UniformBufferPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace EngineUtility;
using namespace Resources;

namespace EngineCore {
namespace ShaderImpl {

DeferredLightShader::DeferredLightShader(const ShaderParams& params)
    : Shader(params)
{
    ShaderInit();
}

DeferredLightShader::~DeferredLightShader()
{
}

void DeferredLightShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    Base::AccessAllUniformLocations(shaderProgramId);
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();

    u_CameraWorldPosition = GetUniform("CameraWorldPosition", shaderProgramId);

    u_gBuffer_Position = GetUniform("gBuffer_Position", shaderProgramId);
    u_gBuffer_Normal = GetUniform("gBuffer_Normal", shaderProgramId);
    u_gBuffer_Albedo = GetUniform("gBuffer_Albedo", shaderProgramId);
#ifdef SHADING_MODEL_PBR
    u_gBuffer_MetallicRoughness = GetUniform("gBuffer_MetallicRoughness", shaderProgramId);
#endif
    u_gBuffer_Emission = GetUniform("gBuffer_Emission", shaderProgramId);

#ifndef NO_LIT

    u_dataBuffer = Resources::UniformBufferPool::GetInstance()->GetOrAllocateResource(
        UniformBufferParameters{"DeferredLightShader_DataBuffer", "LightData", 0, lightData.GetSizeInBytes(), shaderProgramId});

    if (cfg.EnableSpotLights) {
        u_SpotlightAmbientColor
            = GetUniformArray("SpotlightAmbientColor", cfg.MaxSpotlightCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightDiffuseColor
            = GetUniformArray("SpotlightDiffuseColor", cfg.MaxSpotlightCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightSpecularColor
            = GetUniformArray("SpotlightSpecularColor", cfg.MaxSpotlightCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightDirection
            = GetUniformArray("SpotlightDirection", cfg.MaxSpotlightCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightPosition
            = GetUniformArray("SpotlightPosition", cfg.MaxSpotlightCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightCutoff
            = GetUniformArray("SpotlightCutoff", cfg.MaxSpotlightCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightCount = GetUniform("SpotlightCount", shaderProgramId);
    }

    if (cfg.EnableShadows) {
        u_DirectionalLightShadowMaps
            = GetUniformArray("DirLightShadowMaps", lightData.s_maxDirLightCount, shaderProgramId, eShaderType::FragmentShader);
        u_PointLightShadowMaps = GetUniformArray(
            "PointLightShadowMaps", cfg.MaxPointLightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
        u_PointLightShadowProjectionFarPlane = GetUniformArray(
            "PointLightShadowProjectionFarPlane", cfg.MaxPointLightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
        u_PointLightShadowMapCount = GetUniform("PointLightShadowMapCount", shaderProgramId);
        u_SpotlightShadowMaps = GetUniformArray(
            "SpotlightShadowMaps", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightShadowProjectionFarPlane = GetUniformArray(
            "SpotlightShadowProjectionFarPlane", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightShadowMatrices = GetUniformArray(
            "SpotlightShadowMatrices", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightAtlasOffset = GetUniformArray(
            "SpotlightShadowAtlasOffset", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightShadowMapCount = GetUniform("SpotlightShadowMapCount", shaderProgramId);
    }
#endif
}

void DeferredLightShader::SetShaderPredefine()
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();

    DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_COUNT", lightData.s_maxDirLightCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_POINT_LIGHT_COUNT", lightData.s_maxPointLightCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_SPOTLIGHT_COUNT", cfg.MaxSpotlightCount);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_DIR_LIGHT", cfg.ShadowMapBiasDirLight);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_POINT_LIGHT", cfg.ShadowMapBiasPointLight);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_SPOTLIGHT", cfg.ShadowMapBiasSpotlight);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_DIR_LIGHT", cfg.DirLightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_POINT_LIGHT", cfg.PointLightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_SPOTLIGHT", cfg.SpotlightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_SHADOW_MAP_COUNT", lightData.s_maxDirLightCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_POINT_LIGHT_SHADOW_MAP_COUNT", cfg.MaxPointLightShadowMapCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_SPOTLIGHT_SHADOW_MAP_COUNT", cfg.MaxSpotlightShadowMapCount);

    Define(FragmentShader, "GAMMA_CORRECTION");
#ifdef NO_LIT
    Define(FragmentShader, "NO_LIT");
#else
    Undefine(FragmentShader, "NO_LIT");
#endif

#ifdef SHADING_MODEL_PBR
    Define(FragmentShader, "SHADING_MODEL_PBR");
#else
    Undefine(FragmentShader, "SHADING_MODEL_PBR");
#endif
    if (cfg.EnablePointLights) {
        Define(FragmentShader, "ENABLE_POINT_LIGHTING");
    } else {
        Undefine(FragmentShader, "ENABLE_POINT_LIGHTING");
    }

    if (cfg.EnableSpotLights) {
        Define(FragmentShader, "ENABLE_SPOT_LIGHTING");
    } else {
        Undefine(FragmentShader, "ENABLE_SPOT_LIGHTING");
    }

    if (cfg.EnableShadows) {
        Define(FragmentShader, "ENABLE_SHADOWS");
    } else {
        Undefine(FragmentShader, "ENABLE_SHADOWS");
    }
}

void DeferredLightShader::SetCameraWorldPosition(const glm::vec3& cameraWorldPosition)
{
    u_CameraWorldPosition.LoadUniform(cameraWorldPosition);
}

void DeferredLightShader::SetGBufferAlbedo(int32_t slot)
{
    u_gBuffer_Albedo.LoadUniform(slot);
}

void DeferredLightShader::SetGBufferNormal(int32_t slot)
{
    u_gBuffer_Normal.LoadUniform(slot);
}

void DeferredLightShader::SetGBufferPosition(int32_t slot)
{
    u_gBuffer_Position.LoadUniform(slot);
}

#ifdef SHADING_MODEL_PBR
void DeferredLightShader::SetGBufferMetallicRoughness(int32_t slot)
{
    u_gBuffer_MetallicRoughness.LoadUniform(slot);
}
#endif

void DeferredLightShader::SetGBufferEmission(const int32_t slot)
{
    u_gBuffer_Emission.LoadUniform(slot);
}

#ifndef NO_LIT

void DeferredLightShader::SetDirectionalLightShadowMapSlot(size_t index, int32_t slot, const glm::vec4& atlasOffset)
{
    if ((sizeof(lightData.DirectionalLightAtlasOffset) / sizeof(glm::vec4)) > index) {
        lightData.DirectionalLightAtlasOffset[index] = atlasOffset;
    }
    u_DirectionalLightShadowMaps.LoadUniform(index, slot);
}

void DeferredLightShader::SetDirectionalLightShadowMapSlot(size_t index, int32_t slot)
{
    u_DirectionalLightShadowMaps.LoadUniform(index, slot);
}

void DeferredLightShader::SetDirectionalLightShadowMapCount(int32_t count)
{
    lightData.DirectionalLightShadowMapCount = count;
}

void DeferredLightShader::SetDirectionalLightShadowMatrix(size_t index, const glm::mat4& shadowMatrix)
{
    if ((sizeof(lightData.DirectionalLightShadowMatrices) / sizeof(glm::vec4)) > index) {
        lightData.DirectionalLightShadowMatrices[index] = shadowMatrix;
    }
}

void DeferredLightShader::SetPointLightShadowMapSlot(size_t index, int32_t slot)
{
    u_PointLightShadowMaps.LoadUniform(index, slot);
}

void DeferredLightShader::SetPointLightShadowMapCount(int32_t count)
{
    u_PointLightShadowMapCount.LoadUniform(count);
}

void DeferredLightShader::SetPointLightShadowProjectionFarPlane(size_t index, float FarPlane)
{
    u_PointLightShadowProjectionFarPlane.LoadUniform(index, FarPlane);
}

void DeferredLightShader::SetSpotlightShadowMapSlot(size_t index, int32_t slot, const glm::vec4& atlasOffset)
{
    u_SpotlightShadowMaps.LoadUniform(index, slot);
    u_SpotlightAtlasOffset.LoadUniform(index, atlasOffset);
}

void DeferredLightShader::SetSpotlightShadowMapCount(int32_t count)
{
    u_SpotlightShadowMapCount.LoadUniform(count);
}

void DeferredLightShader::SetSpotlightShadowProjectionFarPlane(size_t index, float FarPlane)
{
    u_SpotlightShadowProjectionFarPlane.LoadUniform(index, FarPlane);
}

void DeferredLightShader::SetSpotlightShadowMatrix(size_t index, const glm::mat4& shadowMatrix)
{
    u_SpotlightShadowMatrices.LoadUniform(index, shadowMatrix);
}

void DeferredLightShader::SetLightsInfo(const std::vector<std::shared_ptr<LightSceneProxy>>& lightsProxies)
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();

    // Directional lights
    int32_t dirLightProxyIndex = 0, pointLightProxyIndex = 0, spotlightProxyIndex = 0;
    for (const auto& lightProxy : lightsProxies) {
        if (!lightProxy->IsEnabled() || !lightProxy->IsVisible()) {
            continue;
        }

        if (LightSceneProxyType::DIR_LIGHT == lightProxy->GetLightProxyType()) {
            const auto dirLProxySp = std::static_pointer_cast<DirectionalLightSceneProxy>(lightProxy);
            if (dirLightProxyIndex < lightData.s_maxDirLightCount) {
                lightData.AmbientColors[dirLightProxyIndex] = glm::vec4(dirLProxySp->AmbientColor, 0.0f);
                lightData.DiffuseColors[dirLightProxyIndex] = glm::vec4(dirLProxySp->DiffuseColor, 0.0f);
                lightData.SpecularColors[dirLightProxyIndex] = glm::vec4(dirLProxySp->SpecularColor, 0.0f);
                lightData.Directions[dirLightProxyIndex] = glm::vec4(dirLProxySp->GetDirection(), 0.0f);
                dirLightProxyIndex++;
            }
        }

        if (cfg.EnablePointLights && LightSceneProxyType::POINT_LIGHT == lightProxy->GetLightProxyType()) {
            const auto& pointLProxySp = std::static_pointer_cast<PointLightSceneProxy>(lightProxy);
            if (pointLightProxyIndex < lightData.s_maxPointLightCount) {
                lightData.PointLightDiffuseColor[pointLightProxyIndex] = glm::vec4(pointLProxySp->DiffuseColor, 0.0f);
                lightData.PointLightSpecularColor[pointLightProxyIndex] = glm::vec4(pointLProxySp->SpecularColor, 0.0f);
                lightData.PointLightAttenuation[pointLightProxyIndex] = glm::vec4(pointLProxySp->GetAttenuation(), 0.0f);
                lightData.PointLightPositionWorld[pointLightProxyIndex] = glm::vec4(pointLProxySp->GetPosition(), 0.0f);
                pointLightProxyIndex++;
            }
        }

        if (cfg.EnableSpotLights && LightSceneProxyType::SPOT_LIGHT == lightProxy->GetLightProxyType()) {
            const auto& spotlightProxySp = std::static_pointer_cast<SpotlightSceneProxy>(lightProxy);
            u_SpotlightAmbientColor.LoadUniform(spotlightProxyIndex, spotlightProxySp->AmbientColor);
            u_SpotlightDiffuseColor.LoadUniform(spotlightProxyIndex, spotlightProxySp->DiffuseColor);
            u_SpotlightSpecularColor.LoadUniform(spotlightProxyIndex, spotlightProxySp->SpecularColor);
            u_SpotlightPosition.LoadUniform(spotlightProxyIndex, spotlightProxySp->GetPosition());
            u_SpotlightDirection.LoadUniform(spotlightProxyIndex, spotlightProxySp->GetDirection());
            u_SpotlightCutoff.LoadUniform(spotlightProxyIndex, spotlightProxySp->GetCutoff());
            spotlightProxyIndex++;
        }
    }

    lightData.DirectionalLightCount = dirLightProxyIndex;
    lightData.PointLightCount = pointLightProxyIndex;
    u_SpotlightCount.LoadUniform(spotlightProxyIndex);

    u_dataBuffer->SetData(lightData.GetRawData(), lightData.GetSizeInBytes());
    u_dataBuffer->BindUniformBuffer();
}

#endif

} // namespace ShaderImpl
} // namespace EngineCore
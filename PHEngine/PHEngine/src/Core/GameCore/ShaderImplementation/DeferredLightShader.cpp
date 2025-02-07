#include "DeferredLightShader.h"

#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace EngineUtility;

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
    u_PointLightDiffuseColor
        = GetUniformArray("PointLightDiffuseColor", cfg.MaxPointLightCount, shaderProgramId, eShaderType::FragmentShader);
    u_PointLightSpecularColor
        = GetUniformArray("PointLightSpecularColor", cfg.MaxPointLightCount, shaderProgramId, eShaderType::FragmentShader);
    u_PointLightAttenuation
        = GetUniformArray("PointLightAttenuation", cfg.MaxPointLightCount, shaderProgramId, eShaderType::FragmentShader);

    u_PointLightShadowMaps
        = GetUniformArray("PointLightShadowMaps", cfg.MaxPointLightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_PointLightPositionWorld
        = GetUniformArray("PointLightPositionWorld", cfg.MaxPointLightCount, shaderProgramId, eShaderType::FragmentShader);
    u_PointLightShadowProjectionFarPlane = GetUniformArray(
        "PointLightShadowProjectionFarPlane", cfg.MaxPointLightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_PointLightShadowMapCount = GetUniform("PointLightShadowMapCount", shaderProgramId);
    u_PointLightCount = GetUniform("PointLightCount", shaderProgramId);

    u_DirLightAmbientColor
        = GetUniformArray("DirLightAmbientColor", cfg.MaxDirLightCount, shaderProgramId, eShaderType::FragmentShader);
    u_DirLightDiffuseColor
        = GetUniformArray("DirLightDiffuseColor", cfg.MaxDirLightCount, shaderProgramId, eShaderType::FragmentShader);
    u_DirLightSpecularColor
        = GetUniformArray("DirLightSpecularColor", cfg.MaxDirLightCount, shaderProgramId, eShaderType::FragmentShader);
    u_DirLightDirection
        = GetUniformArray("DirLightDirection", cfg.MaxDirLightCount, shaderProgramId, eShaderType::FragmentShader);

    u_DirectionalLightShadowMaps
        = GetUniformArray("DirLightShadowMaps", cfg.MaxDirLightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_DirectionalLightShadowMatrices
        = GetUniformArray("DirLightShadowMatrices", cfg.MaxDirLightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_DirectionalLightAtlasOffset = GetUniformArray(
        "DirLightShadowAtlasOffset", cfg.MaxDirLightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_DirectionalLightShadowMapCount = GetUniform("DirLightShadowMapCount", shaderProgramId);
    u_DirectionalLightCount = GetUniform("DirLightCount", shaderProgramId);

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
    u_SpotlightCutoff = GetUniformArray("SpotlightCutoff", cfg.MaxSpotlightCount, shaderProgramId, eShaderType::FragmentShader);

    u_SpotlightShadowMaps
        = GetUniformArray("SpotlightShadowMaps", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_SpotlightShadowProjectionFarPlane = GetUniformArray(
        "SpotlightShadowProjectionFarPlane", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_SpotlightShadowMatrices = GetUniformArray(
        "SpotlightShadowMatrices", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_SpotlightAtlasOffset = GetUniformArray(
        "SpotlightShadowAtlasOffset", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    u_SpotlightShadowMapCount = GetUniform("SpotlightShadowMapCount", shaderProgramId);
    u_SpotlightCount = GetUniform("SpotlightCount", shaderProgramId);
#endif
}

void DeferredLightShader::SetShaderPredefine()
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();

    DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_COUNT", cfg.MaxDirLightCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_POINT_LIGHT_COUNT", cfg.MaxPointLightCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_SPOTLIGHT_COUNT", cfg.MaxSpotlightCount);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_DIR_LIGHT", cfg.ShadowMapBiasDirLight);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_POINT_LIGHT", cfg.ShadowMapBiasPointLight);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_SPOTLIGHT", cfg.ShadowMapBiasSpotlight);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_DIR_LIGHT", cfg.DirLightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_POINT_LIGHT", cfg.PointLightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_SPOTLIGHT", cfg.SpotlightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_SHADOW_MAP_COUNT", cfg.MaxDirLightShadowMapCount);
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
    u_DirectionalLightShadowMaps.LoadUniform(index, slot);
    u_DirectionalLightAtlasOffset.LoadUniform(index, atlasOffset);
}

void DeferredLightShader::SetDirectionalLightShadowMapSlot(size_t index, int32_t slot)
{
    u_DirectionalLightShadowMaps.LoadUniform(index, slot);
}

void DeferredLightShader::SetDirectionalLightShadowMapCount(int32_t count)
{
    u_DirectionalLightShadowMapCount.LoadUniform(count);
}

void DeferredLightShader::SetDirectionalLightShadowMatrix(size_t index, const glm::mat4& shadowMatrix)
{
    u_DirectionalLightShadowMatrices.LoadUniform(index, shadowMatrix);
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
    // Directional lights
    int32_t dirLightProxyIndex = 0;
    for (const auto& lightProxy : lightsProxies) {
        if (lightProxy->IsEnabled() && lightProxy->IsVisible()
            && lightProxy->GetLightProxyType() == LightSceneProxyType::DIR_LIGHT) {
            const auto& dirLProxySp = std::static_pointer_cast<DirectionalLightSceneProxy>(lightProxy);
            u_DirLightAmbientColor.LoadUniform(dirLightProxyIndex, dirLProxySp->AmbientColor);
            u_DirLightDiffuseColor.LoadUniform(dirLightProxyIndex, dirLProxySp->DiffuseColor);
            u_DirLightSpecularColor.LoadUniform(dirLightProxyIndex, dirLProxySp->SpecularColor);
            u_DirLightDirection.LoadUniform(dirLightProxyIndex, dirLProxySp->GetDirection());

            dirLightProxyIndex++;
        }
    }
    u_DirectionalLightCount.LoadUniform(dirLightProxyIndex);

    // Point lights
    int32_t pointLightProxyIndex = 0;
    for (const auto& lightProxy : lightsProxies) {
        if (lightProxy->IsEnabled() && lightProxy->IsVisible()
            && lightProxy->GetLightProxyType() == LightSceneProxyType::POINT_LIGHT) {
            const auto& pointLProxySp = std::static_pointer_cast<PointLightSceneProxy>(lightProxy);
            u_PointLightDiffuseColor.LoadUniform(pointLightProxyIndex, pointLProxySp->DiffuseColor);
            u_PointLightSpecularColor.LoadUniform(pointLightProxyIndex, pointLProxySp->SpecularColor);
            u_PointLightPositionWorld.LoadUniform(pointLightProxyIndex, pointLProxySp->GetPosition());
            u_PointLightAttenuation.LoadUniform(pointLightProxyIndex, pointLProxySp->GetAttenuation());

            pointLightProxyIndex++;
        }
    }
    u_PointLightCount.LoadUniform(pointLightProxyIndex);

    // Spotlights
    int32_t spotlightProxyIndex = 0;
    for (const auto& lightProxy : lightsProxies) {
        if (lightProxy->IsEnabled() && lightProxy->IsVisible()
            && lightProxy->GetLightProxyType() == LightSceneProxyType::SPOT_LIGHT) {
            const auto& spotlightProxySp = std::static_pointer_cast<SpotlightSceneProxy>(lightProxy);
            u_SpotlightAmbientColor.LoadUniform(spotlightProxyIndex, spotlightProxySp->AmbientColor);
            u_SpotlightDiffuseColor.LoadUniform(spotlightProxyIndex, spotlightProxySp->DiffuseColor);
            u_SpotlightSpecularColor.LoadUniform(spotlightProxyIndex, spotlightProxySp->SpecularColor);
            u_SpotlightPosition.LoadUniform(spotlightProxyIndex, spotlightProxySp->GetPosition());
            u_SpotlightDirection.LoadUniform(spotlightProxyIndex, spotlightProxySp->GetDirection());
            u_SpotlightCutoff.LoadUniform(spotlightProxyIndex, spotlightProxySp->GetCutoff());
            // todo: u_SpotlightAttenuation.LoadUniform(spotlightProxyIndex, spotlightProxySp->GetAttenuation());

            spotlightProxyIndex++;
        }
    }
    u_SpotlightCount.LoadUniform(spotlightProxyIndex);
}

#endif

} // namespace ShaderImpl
} // namespace EngineCore
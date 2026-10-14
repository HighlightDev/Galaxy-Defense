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

    u_gBuffer_Position = GetUniform("gBuffer_Position", shaderProgramId);
    u_gBuffer_Normal = GetUniform("gBuffer_Normal", shaderProgramId);
    u_gBuffer_Albedo = GetUniform("gBuffer_Albedo", shaderProgramId);
#ifdef SHADING_MODEL_PBR
    u_gBuffer_MetallicRoughness = GetUniform("gBuffer_MetallicRoughness", shaderProgramId);
#endif
    u_gBuffer_Emission = GetUniform("gBuffer_Emission", shaderProgramId);

#ifndef NO_LIT

    u_dataBuffer = Resources::UniformBufferPool::GetInstance()->GetOrAllocateResource(
        UniformBufferParameters{"DeferredLightShader_DataBuffer", "LightData", 0, mLightData.GetSizeInBytes(), shaderProgramId});

    if (cfg.EnableShadows) {
        u_DirectionalLightShadowMaps
            = GetUniformArray("DirLightShadowMaps", mLightData.s_maxDirLightCount, shaderProgramId, eShaderType::FragmentShader);
        u_PointLightShadowMaps = GetUniformArray(
            "PointLightShadowMaps", cfg.MaxPointLightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
        u_SpotlightShadowMaps = GetUniformArray(
            "SpotlightShadowMaps", cfg.MaxSpotlightShadowMapCount, shaderProgramId, eShaderType::FragmentShader);
    }
#endif
}

void DeferredLightShader::SetShaderPredefine()
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();

    DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_COUNT", mLightData.s_maxDirLightCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_POINT_LIGHT_COUNT", mLightData.s_maxPointLightCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_SPOTLIGHT_COUNT", mLightData.s_maxSpotLightCount);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_DIR_LIGHT", cfg.ShadowMapBiasDirLight);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_POINT_LIGHT", cfg.ShadowMapBiasPointLight);
    DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_SPOTLIGHT", cfg.ShadowMapBiasSpotlight);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_DIR_LIGHT", cfg.DirLightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_POINT_LIGHT", cfg.PointLightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_SPOTLIGHT", cfg.SpotlightPCFSamplesCount);
    DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_SHADOW_MAP_COUNT", mLightData.s_maxDirLightCount);
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

    if (cfg.EnableShadows) {
        Define(FragmentShader, "ENABLE_SHADOWS");
    } else {
        Undefine(FragmentShader, "ENABLE_SHADOWS");
    }
}

void DeferredLightShader::SetCameraWorldPosition(const glm::vec3& cameraWorldPosition)
{
    mLightData.CameraWorldPosition = glm::vec4(cameraWorldPosition, 0.0f);
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
    if (std::size(mLightData.DirectionalLightAtlasOffset) > index) {
        mLightData.DirectionalLightAtlasOffset[index] = atlasOffset;
    }
    u_DirectionalLightShadowMaps.LoadUniform(index, slot);
}

void DeferredLightShader::SetPointLightShadowMapSlot(size_t index, int32_t slot)
{
    u_PointLightShadowMaps.LoadUniform(index, slot);
}

void DeferredLightShader::SetSpotlightShadowMapSlot(size_t index, int32_t slot, const glm::vec4& atlasOffset)
{
    if (std::size(mLightData.SpotlightShadowAtlasOffset) > index) {
        mLightData.SpotlightShadowAtlasOffset[index] = atlasOffset;
    }
    u_SpotlightShadowMaps.LoadUniform(index, slot);
}

void DeferredLightShader::SetLightsInfo(const std::vector<std::shared_ptr<LightSceneProxy>>& lightsProxies)
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();

    int32_t dirLightProxyIndex = 0, pointLightProxyIndex = 0, spotLightProxyIndex = 0, dirLightShadowmapIndex = 0,
            pointLightShadowmapIndex = 0, spotLightShadowmapIndex = 0;
    for (const auto& lightProxy : lightsProxies) {
        if (!lightProxy->IsEnabled() || !lightProxy->IsVisible()) {
            continue;
        }

        if (LightSceneProxyType::DIR_LIGHT == lightProxy->GetLightProxyType()) {
            const auto dirLProxySp = std::static_pointer_cast<DirectionalLightSceneProxy>(lightProxy);
            if (dirLightProxyIndex < mLightData.s_maxDirLightCount) {
                mLightData.AmbientColors[dirLightProxyIndex] = glm::vec4(dirLProxySp->AmbientColor, 0.0f);
                mLightData.DiffuseColors[dirLightProxyIndex] = glm::vec4(dirLProxySp->DiffuseColor, 0.0f);
                mLightData.SpecularColors[dirLightProxyIndex] = glm::vec4(dirLProxySp->SpecularColor, 0.0f);
                mLightData.Directions[dirLightProxyIndex] = glm::vec4(dirLProxySp->GetDirection(), 0.0f);
                if (const auto shadowInfo = dirLProxySp->GetProjectedDirShadowInfo()) {
                    mLightData.DirectionalLightShadowMatrices[dirLightProxyIndex] = shadowInfo->GetShadowMatrix();
                    ++dirLightShadowmapIndex;
                }

                ++dirLightProxyIndex;
            }
        }

        if (cfg.EnablePointLights && LightSceneProxyType::POINT_LIGHT == lightProxy->GetLightProxyType()) {
            const auto& pointLProxySp = std::static_pointer_cast<PointLightSceneProxy>(lightProxy);
            if (pointLightProxyIndex < mLightData.s_maxPointLightCount) {
                mLightData.PointLightDiffuseColor[pointLightProxyIndex] = glm::vec4(pointLProxySp->DiffuseColor, 0.0f);
                mLightData.PointLightSpecularColor[pointLightProxyIndex] = glm::vec4(pointLProxySp->SpecularColor, 0.0f);
                mLightData.PointLightAttenuation[pointLightProxyIndex] = glm::vec4(pointLProxySp->GetAttenuation(), 0.0f);
                mLightData.PointLightPositionWorld[pointLightProxyIndex] = glm::vec4(pointLProxySp->GetPosition(), 0.0f);
                if (const auto shadowInfo = pointLProxySp->GetShadowInfo()) {
                    mLightData.PointLightShadowProjectionFarPlane[pointLightProxyIndex].x = pointLProxySp->GetRadianceRadius();
                    ++pointLightShadowmapIndex;
                }

                ++pointLightProxyIndex;
            }
        }

        if (LightSceneProxyType::SPOT_LIGHT == lightProxy->GetLightProxyType()) {
            const auto& spotlightProxySp = std::static_pointer_cast<SpotlightSceneProxy>(lightProxy);
            if (spotLightProxyIndex < mLightData.s_maxSpotLightCount) {
                mLightData.SpotlightAmbientColor[spotLightProxyIndex] = glm::vec4(spotlightProxySp->AmbientColor, 0.0f);
                mLightData.SpotlightDiffuseColor[spotLightProxyIndex] = glm::vec4(spotlightProxySp->DiffuseColor, 0.0f);
                mLightData.SpotlightSpecularColor[spotLightProxyIndex] = glm::vec4(spotlightProxySp->SpecularColor, 0.0f);
                mLightData.SpotlightPosition[spotLightProxyIndex] = glm::vec4(spotlightProxySp->GetPosition(), 0.0f);
                mLightData.SpotlightDirection[spotLightProxyIndex] = glm::vec4(spotlightProxySp->GetDirection(), 0.0f);
                mLightData.SpotlightCutoff[spotLightProxyIndex] = glm::vec4(spotlightProxySp->GetCutoff(), 0.0f, 0.0f, 0.0f);
                if (const auto shadowInfo = spotlightProxySp->GetProjectedSpotLightShadowInfo()) {
                    mLightData.SpotlightShadowMatrices[spotLightProxyIndex] = shadowInfo->GetShadowMatrix();
                    mLightData.SpotLightShadowProjectionFarPlane[spotLightProxyIndex].x = spotlightProxySp->GetRadianceRadius();
                    ++spotLightShadowmapIndex;
                }
            }
            ++spotLightProxyIndex;
        }
    }

    mLightData.LightsCount = glm::ivec4(dirLightProxyIndex, pointLightProxyIndex, spotLightProxyIndex, 0);
    mLightData.ShadowMapsCount = glm::ivec4(dirLightShadowmapIndex, pointLightShadowmapIndex, spotLightShadowmapIndex, 0);
    u_dataBuffer->SetData(mLightData.GetRawData(), mLightData.GetSizeInBytes());
    u_dataBuffer->BindUniformBuffer();
}

#endif

} // namespace ShaderImpl
} // namespace EngineCore
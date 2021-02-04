#include "DeferredLightShader.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/SpotlightSceneProxy.h"
#include "Core/GameCore/GlobalSettings.h"

namespace Game
{
   namespace ShaderImpl
   {

      DeferredLightShader::DeferredLightShader(const ShaderParams& params)
         : ShaderBase(params)
         , MAX_POINT_LIGHT_COUNT(GlobalSettings::GetInstance()->GetMaxPointLightCount())
         , MAX_DIR_LIGHT_COUNT(GlobalSettings::GetInstance()->GetMaxDirLightCount())
         , MAX_SPOTLIGHT_COUNT(GlobalSettings::GetInstance()->GetMaxSpotlightCount())
      {   
         ShaderInit();
      }

      DeferredLightShader::~DeferredLightShader()
      {

      }

      void DeferredLightShader::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         Base::AccessAllUniformLocations(shaderProgramId);

         u_CameraWorldPosition = GetUniform("CameraWorldPosition", shaderProgramId);

         u_gBuffer_Position = GetUniform("gBuffer_Position", shaderProgramId);
         u_gBuffer_Normal = GetUniform("gBuffer_Normal", shaderProgramId);
         u_gBuffer_AlbedoNSpecular = GetUniform("gBuffer_AlbedoNSpecular", shaderProgramId);

#ifndef NO_LIT
         u_PointLightDiffuseColor = GetUniformArray("PointLightDiffuseColor", MAX_POINT_LIGHT_COUNT, shaderProgramId);
         u_PointLightSpecularColor = GetUniformArray("PointLightSpecularColor", MAX_POINT_LIGHT_COUNT, shaderProgramId);
         u_PointLightAttenuation = GetUniformArray("PointLightAttenuation", MAX_POINT_LIGHT_COUNT, shaderProgramId);

         u_PointLightShadowMaps = GetUniformArray("PointLightShadowMaps", GlobalSettings::GetInstance()->GetMaxPointLightShadowMapCount(), shaderProgramId);
         u_PointLightPositionWorld = GetUniformArray("PointLightPositionWorld", MAX_POINT_LIGHT_COUNT, shaderProgramId);
         u_PointLightShadowProjectionFarPlane = GetUniformArray("PointLightShadowProjectionFarPlane", GlobalSettings::GetInstance()->GetMaxPointLightShadowMapCount(), shaderProgramId);
         u_PointLightShadowMapCount = GetUniform("PointLightShadowMapCount", shaderProgramId);
         u_PointLightCount = GetUniform("PointLightCount", shaderProgramId);

         u_DirLightAmbientColor = GetUniformArray("DirLightAmbientColor", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirLightDiffuseColor = GetUniformArray("DirLightDiffuseColor", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirLightSpecularColor = GetUniformArray("DirLightSpecularColor", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirLightDirection = GetUniformArray("DirLightDirection", MAX_DIR_LIGHT_COUNT, shaderProgramId);

         u_DirectionalLightShadowMaps = GetUniformArray("DirLightShadowMaps", GlobalSettings::GetInstance()->GetMaxDirLightShadowMapCount(), shaderProgramId);
         u_DirectionalLightShadowMatrices = GetUniformArray("DirLightShadowMatrices", GlobalSettings::GetInstance()->GetMaxDirLightShadowMapCount(), shaderProgramId);
         u_DirectionalLightAtlasOffset = GetUniformArray("DirLightShadowAtlasOffset", GlobalSettings::GetInstance()->GetMaxDirLightShadowMapCount(), shaderProgramId);
         u_DirectionalLightShadowMapCount = GetUniform("DirLightShadowMapCount", shaderProgramId);
         u_DirectionalLightCount = GetUniform("DirLightCount", shaderProgramId);

         u_SpotlightAmbientColor = GetUniformArray("SpotlightAmbientColor", MAX_SPOTLIGHT_COUNT, shaderProgramId);
         u_SpotlightDiffuseColor = GetUniformArray("SpotlightDiffuseColor", MAX_SPOTLIGHT_COUNT, shaderProgramId);
         u_SpotlightSpecularColor = GetUniformArray("SpotlightSpecularColor", MAX_SPOTLIGHT_COUNT, shaderProgramId);
         u_SpotlightDirection = GetUniformArray("SpotlightDirection", MAX_SPOTLIGHT_COUNT, shaderProgramId);
         u_SpotlightPosition = GetUniformArray("SpotlightPosition", MAX_SPOTLIGHT_COUNT, shaderProgramId);
         u_SpotlightCutoff = GetUniformArray("SpotlightCutoff", MAX_SPOTLIGHT_COUNT, shaderProgramId);

         u_SpotlightShadowMaps = GetUniformArray("SpotlightShadowMaps", GlobalSettings::GetInstance()->GetMaxSpotlightShadowMapCount(), shaderProgramId);
         u_SpotlightShadowProjectionFarPlane = GetUniformArray("SpotlightShadowProjectionFarPlane", GlobalSettings::GetInstance()->GetMaxSpotlightShadowMapCount(), shaderProgramId);
         u_SpotlightShadowMatrices = GetUniformArray("SpotlightShadowMatrices", GlobalSettings::GetInstance()->GetMaxSpotlightShadowMapCount(), shaderProgramId);
         u_SpotlightAtlasOffset = GetUniformArray("SpotlightShadowAtlasOffset", GlobalSettings::GetInstance()->GetMaxSpotlightShadowMapCount(), shaderProgramId);
         u_SpotlightShadowMapCount = GetUniform("SpotlightShadowMapCount", shaderProgramId);
         u_SpotlightCount = GetUniform("SpotlightCount", shaderProgramId);
#endif

#ifdef SHADING_MODEL_PBR
         u_MaterialMetallic = GetUniform("Metallic", shaderProgramId);
         u_MaterialRoughness = GetUniform("Roughness", shaderProgramId);
#endif
      }

      void DeferredLightShader::SetShaderPredefine()
      {
         DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_COUNT", GlobalSettings::GetInstance()->GetMaxDirLightCount());
         DefineConstant<int32_t>(FragmentShader, "MAX_POINT_LIGHT_COUNT", GlobalSettings::GetInstance()->GetMaxPointLightCount());
         DefineConstant<int32_t>(FragmentShader, "MAX_SPOTLIGHT_COUNT", GlobalSettings::GetInstance()->GetMaxSpotlightCount());
         DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_DIR_LIGHT", GlobalSettings::GetInstance()->GetShadowMapBiasDirLight());
         DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_POINT_LIGHT", GlobalSettings::GetInstance()->GetShadowMapBiasPointLight());
         DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_SPOTLIGHT", GlobalSettings::GetInstance()->GetShadowMapBiasSpotlight());
         DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_DIR_LIGHT", GlobalSettings::GetInstance()->GetDirLightPCFSamplesCount());
         DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_POINT_LIGHT", GlobalSettings::GetInstance()->GetPointLightPCFSamplesCount());
         DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_SPOTLIGHT", GlobalSettings::GetInstance()->GetSpotlightPCFSamplesCount());
         DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_SHADOW_MAP_COUNT", GlobalSettings::GetInstance()->GetMaxDirLightShadowMapCount());
         DefineConstant<int32_t>(FragmentShader, "MAX_POINT_LIGHT_SHADOW_MAP_COUNT", GlobalSettings::GetInstance()->GetMaxPointLightShadowMapCount());
         DefineConstant<int32_t>(FragmentShader, "MAX_SPOTLIGHT_SHADOW_MAP_COUNT", GlobalSettings::GetInstance()->GetMaxSpotlightShadowMapCount());
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
      }

      void DeferredLightShader::SetCameraWorldPosition(const glm::vec3& cameraWorldPosition)
      {
         u_CameraWorldPosition.LoadUniform(cameraWorldPosition);
      }

      void DeferredLightShader::SetGBufferAlbedoNSpecular(int32_t slot)
      {
         u_gBuffer_AlbedoNSpecular.LoadUniform(slot);
      }

      void DeferredLightShader::SetGBufferNormal(int32_t slot)
      {
         u_gBuffer_Normal.LoadUniform(slot);
      }

      void DeferredLightShader::SetGBufferPosition(int32_t slot)
      {
         u_gBuffer_Position.LoadUniform(slot);
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

      void DeferredLightShader::SetLightsInfo(const std::unordered_map<size_t /*proxy id*/, std::shared_ptr<LightSceneProxy>>& lightsProxies)
      {
         // Directional lights
         int32_t dirLightProxyIndex = 0;
         for (const auto& lightProxyPair : lightsProxies)
         {
            auto lightProxy = lightProxyPair.second;
            if (lightProxy->GetLightProxyType() == LightSceneProxyType::DIR_LIGHT && dirLightProxyIndex < MAX_DIR_LIGHT_COUNT)
            {
               DirectionalLightSceneProxy* dirLProxyPtr = static_cast<DirectionalLightSceneProxy*>(lightProxy.get());
               u_DirLightAmbientColor.LoadUniform(dirLightProxyIndex, dirLProxyPtr->AmbientColor);
               u_DirLightDiffuseColor.LoadUniform(dirLightProxyIndex, dirLProxyPtr->DiffuseColor);
               u_DirLightSpecularColor.LoadUniform(dirLightProxyIndex, dirLProxyPtr->SpecularColor);
               u_DirLightDirection.LoadUniform(dirLightProxyIndex, dirLProxyPtr->GetDirection());

               dirLightProxyIndex++;
            }
         }
         u_DirectionalLightCount.LoadUniform(dirLightProxyIndex);

         // Point lights
         int32_t pointLightProxyIndex = 0;
         for (const auto& lightProxyPair : lightsProxies)
         {
            auto lightProxy = lightProxyPair.second;
            if (lightProxy->GetLightProxyType() == LightSceneProxyType::POINT_LIGHT && pointLightProxyIndex < MAX_POINT_LIGHT_COUNT)
            {
               PointLightSceneProxy* pointLProxyPtr = static_cast<PointLightSceneProxy*>(lightProxy.get());
               u_PointLightDiffuseColor.LoadUniform(pointLightProxyIndex, pointLProxyPtr->DiffuseColor);
               u_PointLightSpecularColor.LoadUniform(pointLightProxyIndex, pointLProxyPtr->SpecularColor);
               u_PointLightPositionWorld.LoadUniform(pointLightProxyIndex, pointLProxyPtr->GetPosition());
               u_PointLightAttenuation.LoadUniform(pointLightProxyIndex, pointLProxyPtr->GetAttenuation());

               pointLightProxyIndex++;
            }
         }
         u_PointLightCount.LoadUniform(pointLightProxyIndex);

         // Spotlights
         int32_t spotlightProxyIndex = 0;
         for (const auto& lightProxyPair : lightsProxies)
         {
            auto lightProxy = lightProxyPair.second;
            if (lightProxy->GetLightProxyType() == LightSceneProxyType::SPOT_LIGHT && spotlightProxyIndex < MAX_SPOTLIGHT_COUNT)
            {
               SpotlightSceneProxy* spotlightProxyPtr = static_cast<SpotlightSceneProxy*>(lightProxy.get());
               u_SpotlightAmbientColor.LoadUniform(spotlightProxyIndex, spotlightProxyPtr->AmbientColor);
               u_SpotlightDiffuseColor.LoadUniform(spotlightProxyIndex, spotlightProxyPtr->DiffuseColor);
               u_SpotlightSpecularColor.LoadUniform(spotlightProxyIndex, spotlightProxyPtr->SpecularColor);
               u_SpotlightPosition.LoadUniform(spotlightProxyIndex, spotlightProxyPtr->GetPosition());
               u_SpotlightDirection.LoadUniform(spotlightProxyIndex, spotlightProxyPtr->GetDirection());
               u_SpotlightCutoff.LoadUniform(spotlightProxyIndex, spotlightProxyPtr->GetCutoff());
               // todo: u_SpotlightAttenuation.LoadUniform(spotlightProxyIndex, spotlightProxyPtr->GetAttenuation());

               spotlightProxyIndex++;
            }
         }
         u_SpotlightCount.LoadUniform(spotlightProxyIndex);
      }

#endif

#ifdef SHADING_MODEL_PBR

      void DeferredLightShader::SetMaterialMetallic(const float metallic)
      {
         u_MaterialMetallic.LoadUniform(metallic);
      }

      void DeferredLightShader::SetMaterialRoughness(const float roughness)
      {
         u_MaterialRoughness.LoadUniform(roughness);
      }

#endif

   }
}
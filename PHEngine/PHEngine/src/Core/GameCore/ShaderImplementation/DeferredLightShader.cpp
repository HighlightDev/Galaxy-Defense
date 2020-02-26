#include "DeferredLightShader.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"

namespace Game
{
   namespace ShaderImpl
   {

      DeferredLightShader::DeferredLightShader(const ShaderParams& params)
         : ShaderBase(params)
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

         u_PointLightDiffuseColor = GetUniformArray("PointLightDiffuseColor", MAX_POINT_LIGHT_COUNT, shaderProgramId);
         u_PointLightSpecularColor = GetUniformArray("PointLightSpecularColor", MAX_POINT_LIGHT_COUNT, shaderProgramId);
         u_PointLightAttenuation = GetUniformArray("PointLightAttenuation", MAX_POINT_LIGHT_COUNT, shaderProgramId);

         u_PointLightShadowMaps = GetUniformArray("PointLightShadowMaps", MAX_POINT_LIGHT_COUNT, shaderProgramId);
         u_PointLightPositionWorld = GetUniformArray("PointLightPositionWorld", MAX_POINT_LIGHT_COUNT, shaderProgramId);
         u_PointLightShadowMapCount = GetUniform("PointLightShadowMapCount", shaderProgramId);
         u_PointLightCount = GetUniform("PointLightCount", shaderProgramId);
         u_PointLightShadowProjectionFarPlane = GetUniformArray("PointLightShadowProjectionFarPlane", MAX_POINT_LIGHT_COUNT, shaderProgramId);

         u_DirLightAmbientColor = GetUniformArray("DirLightAmbientColor", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirLightDiffuseColor = GetUniformArray("DirLightDiffuseColor", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirLightSpecularColor = GetUniformArray("DirLightSpecularColor", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirLightDirection = GetUniformArray("DirLightDirection", MAX_DIR_LIGHT_COUNT, shaderProgramId);

         u_DirectionalLightShadowMaps = GetUniformArray("DirLightShadowMaps", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirectionalLightShadowMatrices = GetUniformArray("DirLightShadowMatrices", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirectionalLightAtlasOffset = GetUniformArray("DirLightShadowAtlasOffset", MAX_DIR_LIGHT_COUNT, shaderProgramId);
         u_DirectionalLightShadowMapCount = GetUniform("DirLightShadowMapCount", shaderProgramId);
         u_DirectionalLightCount = GetUniform("DirLightCount", shaderProgramId);

#ifdef SHADING_MODEL_PBR
         u_MaterialMetallic = GetUniform("Metallic", shaderProgramId);
         u_MaterialRoughness = GetUniform("Roughness", shaderProgramId);
#endif
      }

      void DeferredLightShader::SetShaderPredefine()
      {
         DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_COUNT", MAX_DIR_LIGHT_COUNT);
         DefineConstant<int32_t>(FragmentShader, "MAX_POINT_LIGHT_COUNT", MAX_POINT_LIGHT_COUNT);
         DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_DIR_LIGHT", SHADOWMAP_BIAS_DIR_LIGHT);
         DefineConstant<float>(FragmentShader, "SHADOWMAP_BIAS_POINT_LIGHT", SHADOWMAP_BIAS_POINT_LIGHT);
         DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_DIR_LIGHT", PCF_SAMPLES_DIR_LIGHT);
         DefineConstant<int32_t>(FragmentShader, "PCF_SAMPLES_POINT_LIGHT", PCF_SAMPLES_POINT_LIGHT);
         DefineConstant<int32_t>(FragmentShader, "MAX_POINT_LIGHT_SHADOW_MAP_COUNT", MAX_POINT_LIGHT_SHADOW_MAP_COUNT);
         DefineConstant<int32_t>(FragmentShader, "MAX_DIR_LIGHT_SHADOW_MAP_COUNT", MAX_DIR_LIGHT_SHADOW_MAP_COUNT);
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

      void DeferredLightShader::SetLightsInfo(const std::vector<std::shared_ptr<LightSceneProxy>>& lightsProxies)
      {
         // Directional lights
         size_t dirLightProxyIndex = 0;
         for (size_t lightProxyIndex = 0; lightProxyIndex < lightsProxies.size(); ++lightProxyIndex)
         {
            if (lightsProxies[lightProxyIndex]->GetLightProxyType() == LightSceneProxyType::DIR_LIGHT && dirLightProxyIndex < MAX_DIR_LIGHT_COUNT)
            {
               const auto& sharedLightProxy = lightsProxies[lightProxyIndex];
               DirectionalLightSceneProxy* dirLProxyPtr = static_cast<DirectionalLightSceneProxy*>(sharedLightProxy.get());
               u_DirLightAmbientColor.LoadUniform(dirLightProxyIndex, dirLProxyPtr->AmbientColor);
               u_DirLightDiffuseColor.LoadUniform(dirLightProxyIndex, dirLProxyPtr->DiffuseColor);
               u_DirLightSpecularColor.LoadUniform(dirLightProxyIndex, dirLProxyPtr->SpecularColor);
               u_DirLightDirection.LoadUniform(dirLightProxyIndex, dirLProxyPtr->GetDirection());

               dirLightProxyIndex++;
            }
         }

         u_DirectionalLightCount.LoadUniform(dirLightProxyIndex);

         // Point lights
         size_t pointLightProxyIndex = 0;
         for (size_t lightProxyIndex = 0; lightProxyIndex < lightsProxies.size(); ++lightProxyIndex)
         {
            if (lightsProxies[lightProxyIndex]->GetLightProxyType() == LightSceneProxyType::POINT_LIGHT && dirLightProxyIndex < MAX_POINT_LIGHT_COUNT)
            {
               const auto& sharedLightProxy = lightsProxies[lightProxyIndex];
               PointLightSceneProxy* pointLProxyPtr = static_cast<PointLightSceneProxy*>(sharedLightProxy.get());
               u_PointLightDiffuseColor.LoadUniform(pointLightProxyIndex, pointLProxyPtr->DiffuseColor);
               u_PointLightSpecularColor.LoadUniform(pointLightProxyIndex, pointLProxyPtr->SpecularColor);
               u_PointLightPositionWorld.LoadUniform(pointLightProxyIndex, pointLProxyPtr->GetPosition());
               u_PointLightAttenuation.LoadUniform(pointLightProxyIndex, pointLProxyPtr->GetAttenuation());

               pointLightProxyIndex++;
            }
         }

         u_PointLightCount.LoadUniform(pointLightProxyIndex);
      }

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
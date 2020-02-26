#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"

#include <string>

using namespace Graphics::OpenGL;
using namespace Graphics::Proxy;

#define MAX_DIR_LIGHT_COUNT 5
#define MAX_DIR_LIGHT_SHADOW_MAP_COUNT 4
#define MAX_POINT_LIGHT_COUNT 50
#define MAX_POINT_LIGHT_SHADOW_MAP_COUNT 4
#define SHADOWMAP_BIAS_DIR_LIGHT 0.005f
#define SHADOWMAP_BIAS_POINT_LIGHT 0.05f
#define PCF_SAMPLES_DIR_LIGHT 2
#define PCF_SAMPLES_POINT_LIGHT 4

namespace Game
{
   namespace ShaderImpl
   {

      class DeferredLightShader :
         public ShaderBase
      {

         using Base = ShaderBase;

         Uniform u_CameraWorldPosition;

         UniformArray u_DirLightAmbientColor;
         UniformArray u_DirLightDiffuseColor;
         UniformArray u_DirLightSpecularColor;
         UniformArray u_DirLightDirection;

         UniformArray u_DirectionalLightShadowMaps;
         UniformArray u_DirectionalLightShadowMatrices;
         UniformArray u_DirectionalLightAtlasOffset;
         Uniform u_DirectionalLightShadowMapCount;
         Uniform u_DirectionalLightCount;

         UniformArray u_PointLightDiffuseColor;
         UniformArray u_PointLightSpecularColor;
         UniformArray u_PointLightAttenuation;
         UniformArray u_PointLightPositionWorld;

         UniformArray u_PointLightShadowMaps;
         UniformArray u_PointLightShadowProjectionFarPlane;
         Uniform u_PointLightShadowMapCount;
         Uniform u_PointLightCount;

         Uniform u_gBuffer_Position;
         Uniform u_gBuffer_Normal;
         Uniform u_gBuffer_AlbedoNSpecular;

#ifdef SHADING_MODEL_PBR
         Uniform u_MaterialMetallic;
         Uniform u_MaterialRoughness;
#endif

      public:

         DeferredLightShader(const ShaderParams& params);

         virtual ~DeferredLightShader();

         void SetGBufferAlbedoNSpecular(int32_t slot);

         void SetGBufferNormal(int32_t slot);

         void SetGBufferPosition(int32_t slot);

         void SetLightsInfo(const std::vector<std::shared_ptr<LightSceneProxy>>& lightsProxies);

         void SetDirectionalLightShadowMapSlot(size_t index, int32_t slot);
         void SetDirectionalLightShadowMapSlot(size_t index, int32_t slot, const glm::vec4& atlasOffset);
         void SetDirectionalLightShadowMapCount(int32_t count);
         void SetDirectionalLightShadowMatrix(size_t index, const glm::mat4& shadowMatrix);

         void SetPointLightShadowMapSlot(size_t index, int32_t slot);
         void SetPointLightShadowMapCount(int32_t count);
         void SetPointLightShadowProjectionFarPlane(size_t index, float FarPlane);

#ifdef SHADING_MODEL_PBR
         void SetMaterialMetallic(const float metallic);
         void SetMaterialRoughness(const float roughness);
#endif
         void SetCameraWorldPosition(const glm::vec3& cameraWorldPosition);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


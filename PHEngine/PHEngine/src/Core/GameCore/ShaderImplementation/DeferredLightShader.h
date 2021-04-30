#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "Core/GraphicsCore/SceneProxy/LightSceneProxy.h"

#include <string>

using namespace Graphics::OpenGL;
using namespace Graphics::Proxy;

namespace Game
{
   namespace ShaderImpl
   {

      class DeferredLightShader :
         public ShaderBase
      {

         using Base = ShaderBase;

         Uniform u_CameraWorldPosition;

#ifndef NO_LIT
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

#ifdef SHADING_MODEL_PBR
         Uniform u_gBuffer_MetallicRoughness;
#endif

         const int32_t MAX_POINT_LIGHT_COUNT;
         const int32_t MAX_DIR_LIGHT_COUNT;
         const int32_t MAX_SPOTLIGHT_COUNT;

      public:

         DeferredLightShader(const ShaderParams& params);

         virtual ~DeferredLightShader();

         void SetGBufferAlbedo(int32_t slot);

         void SetGBufferNormal(int32_t slot);

         void SetGBufferPosition(int32_t slot);

         void SetGBufferMetallicRoughness(int32_t slot);

#ifndef NO_LIT
         void SetLightsInfo(const std::unordered_map<size_t /*proxy id*/, std::shared_ptr<LightSceneProxy>>& lightsProxies);

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

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


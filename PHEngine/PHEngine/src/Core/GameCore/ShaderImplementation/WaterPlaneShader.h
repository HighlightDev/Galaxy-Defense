#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

using namespace Graphics::OpenGL;

namespace EngineCore
{

   namespace ShaderImpl
   {

      class WaterPlaneShader :
         public Shader
      {
         using Base = Shader;

         Uniform u_modelMatrix, u_viewMatrix, u_projectionMatrix,
            u_reflectionTexture, u_refractionTexture, u_dudvTexture,
            u_normalMap, u_depthTexture, u_cameraPosition,
            u_moveFactor, u_waveStrength, u_sunPos,
            u_sunSpecularColour, u_nearClipPlane, u_farClipPlane,
            u_transparencyDepth, u_mistEnable, u_mistDensity,
            u_mistGradient, u_mistColour, u_bEnableSun;

         void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         void SetShaderPredefine() override;

      public:

         WaterPlaneShader(const ShaderParams& params);

         ~WaterPlaneShader() override;

         void SetTransformationMatrices(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix);

         void SetReflectionSampler(int32_t reflectionSampler);

         void SetRefractionSampler(int32_t refractionSampler);

         void SetDepthSampler(int32_t depthSampler);

         void SetDuDvSampler(int32_t DuDvSampler);

         void SetNormalMapSampler(int32_t normalMapSampler);

         void SetCameraPosition(glm::vec3& cameraPosition);

         void SetDistortionProperties(float moveFactor, float waveStrength);

         void SetClippingPlanes(float perspectiveClipPlaneNear, float perspectiveClipPlaneFar);

         void SetTransparancyDepth(float transparencyDepth);

      };

   }
}

#include "WaterPlaneShader.h"

namespace Game
{
   namespace ShaderImpl
   {

      WaterPlaneShader::WaterPlaneShader(const ShaderParams& params)
         : ShaderBase(params)
      {
         ShaderInit();
      }

      WaterPlaneShader::~WaterPlaneShader()
      {
      }

      void WaterPlaneShader::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         Base::AccessAllUniformLocations(shaderProgramId);

         u_bEnableSun = GetUniform("bEnableSun", shaderProgramId);
         u_modelMatrix = GetUniform("modelMatrix", shaderProgramId);
         u_viewMatrix = GetUniform("viewMatrix", shaderProgramId);
         u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramId);
         u_reflectionTexture = GetUniform("reflectionTexture", shaderProgramId);
         u_refractionTexture = GetUniform("refractionTexture", shaderProgramId);
         u_cameraPosition = GetUniform("cameraPosition", shaderProgramId);
         u_dudvTexture = GetUniform("dudvTexture", shaderProgramId);
         u_normalMap = GetUniform("normalMap", shaderProgramId);
         u_depthTexture = GetUniform("depthTexture", shaderProgramId);
         u_moveFactor = GetUniform("moveFactor", shaderProgramId);
         u_sunPos = GetUniform("sunPos", shaderProgramId);
         u_sunSpecularColour = GetUniform("sunSpecularColour", shaderProgramId);
         u_waveStrength = GetUniform("waveStrength", shaderProgramId);
         u_farClipPlane = GetUniform("farClipPlane", shaderProgramId);
         u_nearClipPlane = GetUniform("nearClipPlane", shaderProgramId);
         u_transparencyDepth = GetUniform("transparencyDepth", shaderProgramId);

         u_mistEnable = GetUniform("mistEnable", shaderProgramId);
         u_mistDensity = GetUniform("mistDensity", shaderProgramId);
         u_mistGradient = GetUniform("mistGradient", shaderProgramId);
         u_mistColour = GetUniform("mistColour", shaderProgramId);
      }

      void WaterPlaneShader::SetShaderPredefine()
      {
         DefineConstant<int32_t>(ShaderType(VertexShader | FragmentShader), "MAX_LIGHT_COUNT", 1);
         DefineConstant<float>(ShaderType(VertexShader), "tiling", 15.0f);
         DefineConstant<float>(ShaderType(FragmentShader), "materialReflectivity", 0.7f);
         DefineConstant<float>(ShaderType(FragmentShader), "materialShineDamper", 100.0f);
      }

      void WaterPlaneShader::SetTransformationMatrices(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
      {
         u_modelMatrix.LoadUniform(modelMatrix);
         u_viewMatrix.LoadUniform(viewMatrix);
         u_projectionMatrix.LoadUniform(projectionMatrix);
      }

      void WaterPlaneShader::SetReflectionSampler(int32_t reflectionSampler)
      {
         u_reflectionTexture.LoadUniform(reflectionSampler);
      }

      void WaterPlaneShader::SetRefractionSampler(int32_t refractionSampler)
      {
         u_refractionTexture.LoadUniform(refractionSampler);
      }

      void WaterPlaneShader::SetDepthSampler(int32_t depthSampler)
      {
         u_depthTexture.LoadUniform(depthSampler);
      }

      void WaterPlaneShader::SetDuDvSampler(int32_t DuDvSampler)
      {
         u_dudvTexture.LoadUniform(DuDvSampler);
      }

      void WaterPlaneShader::SetNormalMapSampler(int32_t normalMapSampler)
      {
         u_normalMap.LoadUniform(normalMapSampler);
      }

      void WaterPlaneShader::SetCameraPosition(glm::vec3& cameraPosition)
      {
         u_cameraPosition.LoadUniform(cameraPosition);
      }

      void WaterPlaneShader::SetDistortionProperties(float moveFactor, float waveStrength)
      {
         u_moveFactor.LoadUniform(moveFactor);
         u_waveStrength.LoadUniform(waveStrength);
      }

      void WaterPlaneShader::SetClippingPlanes(float perspectiveClipPlaneNear, float perspectiveClipPlaneFar)
      {
         u_nearClipPlane.LoadUniform(perspectiveClipPlaneNear);
         u_farClipPlane.LoadUniform(perspectiveClipPlaneFar);
      }

      void WaterPlaneShader::SetTransparancyDepth(float transparencyDepth)
      {
         u_transparencyDepth.LoadUniform(transparencyDepth);
      }

   }
}

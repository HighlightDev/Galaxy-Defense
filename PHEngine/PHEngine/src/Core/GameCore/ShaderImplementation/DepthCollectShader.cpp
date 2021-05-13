#include "DepthCollectShader.h"

namespace Game
{
   namespace ShaderImpl
   {
      DepthCollectShader::DepthCollectShader(const ShaderParams& params)
         : Shader(params)
      {
      }

      void DepthCollectShader::AccessAllUniformLocations(uint32_t shaderProgramId) {

         Shader::AccessAllUniformLocations(shaderProgramId);

         u_lightWorldPosition = GetUniform("lightWorldPosition", shaderProgramId);
         u_shadowDistance = GetUniform("shadowDistance", shaderProgramId);
         u_bWriteDepthLinearly = GetUniform("bWriteDepthLinearly", shaderProgramId);
      }

      void DepthCollectShader::SetLightWorldPosition(const glm::vec3& position)
      {
         u_lightWorldPosition.LoadUniform(position);
      }

      void DepthCollectShader::SetShadowDistance(const float shadowDistance)
      {
         u_shadowDistance.LoadUniform(shadowDistance);
      }

      void DepthCollectShader::SetWriteDepthLinearly(const bool value)
      {
         u_bWriteDepthLinearly.LoadUniform(value);
      }

      void DepthCollectShader::SetShaderPredefine()
      {
      }
   }
}
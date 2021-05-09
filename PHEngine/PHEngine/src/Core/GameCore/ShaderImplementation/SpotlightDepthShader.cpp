#include "SpotlightDepthShader.h"

namespace Game
{
   namespace ShaderImpl
   {
      SpotlightDepthShaderBase::SpotlightDepthShaderBase(const ShaderParams& params)
         : Shader(params)
      {
      }

      void SpotlightDepthShaderBase::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         Shader::AccessAllUniformLocations(shaderProgramId);
         u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
         u_shadowViewMatrix = GetUniform("shadowViewMatrix", shaderProgramId);
         u_shadowProjectionMatrix = GetUniform("shadowProjectionMatrix", shaderProgramId);
         u_spotlightPos = GetUniform("spotlightPos", shaderProgramId);
         u_farPlane = GetUniform("farPlane", shaderProgramId);
      }

      void SpotlightDepthShaderBase::SetTransformationMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         u_worldMatrix.LoadUniform(worldMatrix);
         u_shadowViewMatrix.LoadUniform(viewMatrix);
         u_shadowProjectionMatrix.LoadUniform(projectionMatrix);
      }

      void SpotlightDepthShaderBase::SetSpotlightPosition(const glm::vec3& position)
      {
         u_spotlightPos.LoadUniform(position);
      }

      void SpotlightDepthShaderBase::SetFarPlane(const float distance)
      {
         u_farPlane.LoadUniform(distance);
      }

      // *************************  SpotlightDepthShader (Non Skeletal)  *************************  //
      SpotlightDepthShader<eShaderMeshType::NON_SKELETAL>::SpotlightDepthShader(const ShaderParams& params)
         : SpotlightDepthShaderBase(params)
      {
         ShaderInit();
      }

      void SpotlightDepthShader<eShaderMeshType::NON_SKELETAL>::SetShaderPredefine()
      {
      }

      // *************************  SpotlightDepthShader (Skeletal)  *************************  //
      SpotlightDepthShader<eShaderMeshType::SKELETAL>::SpotlightDepthShader(const ShaderParams& params)
         : SpotlightDepthShaderBase(params)
      {
         ShaderInit();
      }

      void SpotlightDepthShader<eShaderMeshType::SKELETAL>::SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices)
      {
         for (size_t index = 0; index < skinningMatrices.size(); index++)
            u_boneMatrices.LoadUniform(index, skinningMatrices[index]);
      }

#define MaxWeights 4
#define MaxBones 155

      void SpotlightDepthShader<eShaderMeshType::SKELETAL>::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         SpotlightDepthShaderBase::AccessAllUniformLocations(shaderProgramId);
         u_boneMatrices = GetUniformArray("bonesMatrices", MaxBones, shaderProgramId);
      }

      void SpotlightDepthShader<eShaderMeshType::SKELETAL>::SetShaderPredefine()
      {
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxWeights", MaxWeights);
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxBones", MaxBones);
      }

#undef MaxWeights
#undef MaxBones

   }
}

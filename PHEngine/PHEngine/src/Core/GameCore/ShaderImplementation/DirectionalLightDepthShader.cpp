#include "DirectionalLightDepthShader.h"

namespace Game
{
   namespace ShaderImpl
   {

      DirectionalLightDepthShaderBase::DirectionalLightDepthShaderBase(const ShaderParams& params)
         : Shader(params)
      {
      }

      void DirectionalLightDepthShaderBase::AccessAllUniformLocations(uint32_t shaderProgramId) {

         Shader::AccessAllUniformLocations(shaderProgramId);
         u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
         u_shadowViewMatrix = GetUniform("shadowViewMatrix", shaderProgramId);
         u_shadowProjectionMatrix = GetUniform("shadowProjectionMatrix", shaderProgramId);
      }

      void DirectionalLightDepthShaderBase::SetTransformationMatrices(const glm::mat4& worldMatrix, const glm::mat4& shadowViewMatrix, const glm::mat4& shadowProjectionMatrix)
      {
         u_worldMatrix.LoadUniform(worldMatrix);
         u_shadowViewMatrix.LoadUniform(shadowViewMatrix);
         u_shadowProjectionMatrix.LoadUniform(shadowProjectionMatrix);
      }

      DirectionalLightDepthShader<eShaderMeshType::NON_SKELETAL>::DirectionalLightDepthShader(const ShaderParams& params)
         : DirectionalLightDepthShaderBase(params)
      {
         ShaderInit();
      }

      void DirectionalLightDepthShader<eShaderMeshType::NON_SKELETAL>::SetShaderPredefine() 
      {
      }

      /*     SKELETAL MESH DEPTH SHADER   */

#define MaxWeights 4
#define MaxBones 155

      DirectionalLightDepthShader<eShaderMeshType::SKELETAL>::DirectionalLightDepthShader(const ShaderParams& params)
         : DirectionalLightDepthShaderBase(params)
      {
         ShaderInit();
      }

      void DirectionalLightDepthShader<eShaderMeshType::SKELETAL>::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         DirectionalLightDepthShaderBase::AccessAllUniformLocations(shaderProgramId);
         u_boneMatrices = GetUniformArray("bonesMatrices", MaxBones, shaderProgramId);
      }

      void DirectionalLightDepthShader<eShaderMeshType::SKELETAL>::SetShaderPredefine() 
      {
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxWeights", MaxWeights);
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxBones", MaxBones);
      }

#undef MaxWeights
#undef MaxBones

      void DirectionalLightDepthShader<eShaderMeshType::SKELETAL>::SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices)
      {
         for (size_t index = 0; index < skinningMatrices.size(); index++)
            u_boneMatrices.LoadUniform(index, skinningMatrices[index]);
      }
   }
}
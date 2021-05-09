#include "PointLightDepthShader.h"

namespace Game
{
   namespace ShaderImpl
   {
      PointLightDepthShaderBase::PointLightDepthShaderBase(const ShaderParams& params)
         : Shader(params)
      {
      }

      void PointLightDepthShaderBase::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         Shader::AccessAllUniformLocations(shaderProgramId);
         u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
         u_shadowViewMatrices = GetUniformArray("shadowViewMatrices", 6, shaderProgramId);
         u_shadowProjectionMatrices = GetUniformArray("shadowProjectionMatrices", 6, shaderProgramId);
         u_pointLightPos = GetUniform("pointLightPos", shaderProgramId);
         u_farPlane = GetUniform("farPlane", shaderProgramId);
      }

      void PointLightDepthShaderBase::SetTransformationMatrices(const glm::mat4& worldMatrix, const six_mat4x4& viewMatrices, const six_mat4x4& projectionMatrices)
      {
         u_worldMatrix.LoadUniform(worldMatrix);

         for (size_t matIndex  = 0; matIndex < viewMatrices.size(); ++matIndex)
         {
            const auto& viewMatrix = viewMatrices[matIndex];
            u_shadowViewMatrices.LoadUniform(matIndex, viewMatrix);
         }

         for (size_t matIndex = 0; matIndex < projectionMatrices.size(); ++matIndex)
         {
            const auto& projectionMatrix = projectionMatrices[matIndex];
            u_shadowProjectionMatrices.LoadUniform(matIndex, projectionMatrix);
         }
      }

      void PointLightDepthShaderBase::SetPointLightPosition(const glm::vec3& position)
      {
         u_pointLightPos.LoadUniform(position);
      }

      void PointLightDepthShaderBase::SetFarPlane(const float distance)
      {
         u_farPlane.LoadUniform(distance);
      }

      // *************************  PointLightDepthShader (Non Skeletal)  *************************  //
      PointLightDepthShader<eShaderMeshType::NON_SKELETAL>::PointLightDepthShader(const ShaderParams& params)
         : PointLightDepthShaderBase(params)
      {
         ShaderInit();
      }

      void PointLightDepthShader<eShaderMeshType::NON_SKELETAL>::SetShaderPredefine() 
      {
         DefineConstant<int32_t>(ShaderType::GeometryShader, "CubemapFaces", 6);
      }

      // *************************  PointLightDepthShader (Skeletal)  *************************  //
      PointLightDepthShader<eShaderMeshType::SKELETAL>::PointLightDepthShader(const ShaderParams& params)
         : PointLightDepthShaderBase(params)
      {
         ShaderInit();
      }

      void PointLightDepthShader<eShaderMeshType::SKELETAL>::SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices)
      {
         for (size_t index = 0; index < skinningMatrices.size(); index++)
            u_boneMatrices.LoadUniform(index, skinningMatrices[index]);
      }

#define MaxWeights 4
#define MaxBones 155

      void PointLightDepthShader<eShaderMeshType::SKELETAL>::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         PointLightDepthShaderBase::AccessAllUniformLocations(shaderProgramId);
         u_boneMatrices = GetUniformArray("bonesMatrices", MaxBones, shaderProgramId);
      }

      void PointLightDepthShader<eShaderMeshType::SKELETAL>::SetShaderPredefine()
      {
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxWeights", MaxWeights);
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxBones", MaxBones);
         DefineConstant<int32_t>(ShaderType::GeometryShader, "CubemapFaces", 6);
      }

#undef MaxWeights
#undef MaxBones

   }
}

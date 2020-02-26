#include "CubemapDepthShader.h"


namespace Game
{
   namespace ShaderImpl
   {
      CubemapDepthShaderBase::CubemapDepthShaderBase(const ShaderParams& params)
         : ShaderBase(params)
      {
      }

      void CubemapDepthShaderBase::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         ShaderBase::AccessAllUniformLocations(shaderProgramId);
         u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
         u_shadowViewMatrices = GetUniformArray("shadowViewMatrices", 6, shaderProgramId);
         u_shadowProjectionMatrices = GetUniformArray("shadowProjectionMatrices", 6, shaderProgramId);
         u_pointLightPos = GetUniform("pointLightPos", shaderProgramId);
         u_farPlane = GetUniform("farPlane", shaderProgramId);
      }

      void CubemapDepthShaderBase::SetTransformationMatrices(const glm::mat4& worldMatrix, const six_mat4x4& viewMatrices, const six_mat4x4& projectionMatrices)
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

      void CubemapDepthShaderBase::SetPointLightPosition(const glm::vec3& position)
      {
         u_pointLightPos.LoadUniform(position);
      }

      void CubemapDepthShaderBase::SetFarPlane(const float distance)
      {
         u_farPlane.LoadUniform(distance);
      }

      // *************************  CubemapDepthShader (Non Skeletal)  *************************  //
      CubemapDepthShader<false>::CubemapDepthShader(const ShaderParams& params)
         : CubemapDepthShaderBase(params)
      {
         ShaderInit();
      }

      void CubemapDepthShader<false>::SetShaderPredefine() 
      {
         DefineConstant<int32_t>(ShaderType::GeometryShader, "CubemapFaces", 6);
      }

      // *************************  CubemapDepthShader (Skeletal)  *************************  //
      CubemapDepthShader<true>::CubemapDepthShader(const ShaderParams& params)
         : CubemapDepthShaderBase(params)
      {
         ShaderInit();
      }

      void CubemapDepthShader<true>::SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices)
      {
         for (size_t index = 0; index < skinningMatrices.size(); index++)
            u_boneMatrices.LoadUniform(index, skinningMatrices[index]);
      }

#define MaxWeights 3
#define MaxBones 55

      void CubemapDepthShader<true>::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         CubemapDepthShaderBase::AccessAllUniformLocations(shaderProgramId);
         u_boneMatrices = GetUniformArray("bonesMatrices", MaxBones, shaderProgramId);
      }

      void CubemapDepthShader<true>::SetShaderPredefine()
      {
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxWeights", MaxWeights);
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxBones", MaxBones);
         DefineConstant<int32_t>(ShaderType::GeometryShader, "CubemapFaces", 6);
      }

#undef MaxWeights
#undef MaxBones

   }
}

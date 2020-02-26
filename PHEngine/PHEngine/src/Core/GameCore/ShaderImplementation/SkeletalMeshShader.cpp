#include "SkeletalMeshShader.h"

namespace Game
{
   namespace ShaderImpl
   {

      SkeletalMeshShader::SkeletalMeshShader(const ShaderParams& params)
         : ShaderBase(params)
      {
         ShaderInit();
      }

      SkeletalMeshShader::~SkeletalMeshShader()
      {

      }

      void SkeletalMeshShader::SetTransformationMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         u_worldMatrix.LoadUniform(worldMatrix);
         u_viewMatrix.LoadUniform(viewMatrix);
         u_projectionMatrix.LoadUniform(projectionMatrix);
      }

      void SkeletalMeshShader::SetAlbedoTex(int32_t slot)
      {
         u_albedo.LoadUniform(slot);
      }

      void SkeletalMeshShader::SetNormalMapTex(int32_t slot)
      {

      }

      void SkeletalMeshShader::SetSpecularMapTex(int32_t slot)
      {

      }

      void SkeletalMeshShader::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         Base::AccessAllUniformLocations(shaderProgramId);

         u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
         u_viewMatrix = GetUniform("viewMatrix", shaderProgramId);
         u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramId);
         u_albedo = GetUniform("albedoTexture", shaderProgramId);
         u_boneMatrices = GetUniformArray("bonesMatrices", MAX_BONES, shaderProgramId);
      }

      void SkeletalMeshShader::SetSkinningMatrices(const std::vector<glm::mat4>& matrices)
      {
         for (size_t i = 0; i < matrices.size(); i++)
         {
            u_boneMatrices.LoadUniform(i, matrices[i]);
         }
      }

      void SkeletalMeshShader::SetShaderPredefine()
      {
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxWeights", MAX_WEIGHTS);
         DefineConstant<int32_t>(ShaderType::VertexShader, "MaxBones", MAX_BONES);
      }

   }
}
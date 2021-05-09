#include "BillboardShader.h"


namespace Game
{
   namespace ShaderImpl
   {

      BillboardShader::BillboardShader(const ShaderParams& params)
         : Shader(params)
      {
         ShaderInit();
      }


      BillboardShader::~BillboardShader()
      {
      }

      void BillboardShader::SetTransformMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         u_worldMatrix.LoadUniform(worldMatrix);
         u_viewMatrix.LoadUniform(viewMatrix);
         u_projectionMatrix.LoadUniform(projectionMatrix);
      }

      void BillboardShader::SetTexture(int32_t texSlot)
      {
         u_texture.LoadUniform(texSlot);
      }

      void BillboardShader::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         Shader::AccessAllUniformLocations(shaderProgramID);

         u_worldMatrix = GetUniform("worldMatrix", shaderProgramID);
         u_viewMatrix = GetUniform("viewMatrix", shaderProgramID);
         u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
         u_texture = GetUniform("billboardTexture", shaderProgramID);
      }

      void BillboardShader::SetShaderPredefine()
      {
         DefineConstant<float>(ShaderType::GeometryShader, "billboardSize", 1.0f);
      }

   }
}
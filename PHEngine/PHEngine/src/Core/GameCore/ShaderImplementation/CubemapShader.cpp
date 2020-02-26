#include "CubemapShader.h"


namespace Game
{
   namespace ShaderImpl
   {

      CubemapShader::CubemapShader(const ShaderParams& params)
         : ShaderBase(params)
      {
         ShaderInit();
      }


      CubemapShader::~CubemapShader()
      {
      }

      void CubemapShader::SetTransformMatrices(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
      {
         u_worldMatrix.LoadUniform(worldMatrix);
         u_viewMatrix.LoadUniform(viewMatrix);
         u_projectionMatrix.LoadUniform(projectionMatrix);
      }

      void CubemapShader::SetTexture(int32_t texSlot)
      {
         u_texture.LoadUniform(texSlot);
      }

      void CubemapShader::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         ShaderBase::AccessAllUniformLocations(shaderProgramId);

         u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
         u_viewMatrix = GetUniform("viewMatrix", shaderProgramId);
         u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramId);
         u_texture = GetUniform("cubeTexture", shaderProgramId);
      }

      void CubemapShader::SetShaderPredefine()
      {
      }

   }
}
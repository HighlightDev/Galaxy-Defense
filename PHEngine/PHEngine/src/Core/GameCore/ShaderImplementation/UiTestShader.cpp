#include "UiTestShader.h"

namespace EngineCore
{
   namespace ShaderImpl
   {

      UiTestShader::UiTestShader(const ShaderParams &params)
          : Shader(params)
      {
         Base::ShaderInit();
      }

      void UiTestShader::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         Base::AccessAllUniformLocations(shaderProgramId);

         u_image = GetUniform("image", shaderProgramId);
         u_transformMatrix = GetUniform("transformMatrix", shaderProgramId);
         u_opacity = GetUniform("opacity", shaderProgramId);
      }

      void UiTestShader::SetImageTexture(const int32_t texSlot)
      {
         u_image.LoadUniform(texSlot);
      }

      void UiTestShader::SetTransformMatrix(const glm::mat4 &transformMatrix)
      {
         u_transformMatrix.LoadUniform(transformMatrix);
      }

      void UiTestShader::SetOpacity(const float opacity)
      {
         u_opacity.LoadUniform(opacity);
      }

      void UiTestShader::SetShaderPredefine()
      {
      }

   }
}

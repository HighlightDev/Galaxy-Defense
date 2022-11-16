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

         u_color = GetUniform("uiColor", shaderProgramId);
      }

      void UiTestShader::SetColor(const glm::vec4 &color)
      {
         u_color.LoadUniform(color);
      }

      void UiTestShader::SetShaderPredefine()
      {
      }

   }
}

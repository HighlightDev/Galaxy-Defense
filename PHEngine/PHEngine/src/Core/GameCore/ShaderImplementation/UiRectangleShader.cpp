#include "UiRectangleShader.h"

namespace EngineCore
{
   namespace ShaderImpl
   {

      UiRectangleShader::UiRectangleShader(const ShaderParams &params)
          : Shader(params)
      {
         Base::ShaderInit();
      }

      void UiRectangleShader::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         Base::AccessAllUniformLocations(shaderProgramId);

         u_color = GetUniform("color", shaderProgramId);
         u_translation = GetUniform("translation", shaderProgramId);
         u_scale = GetUniform("scale", shaderProgramId);
      }

      void UiRectangleShader::SetColor(const glm::vec4& color)
      {
         u_color.LoadUniform(color);
      }

      void UiRectangleShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
      {
         u_translation.LoadUniform(normalizedTranslation);
         u_scale.LoadUniform(normalizedScale);
      }

      void UiRectangleShader::SetShaderPredefine()
      {
      }

   }
}

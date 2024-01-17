#include "UiImageShader.h"

namespace EngineCore
{
   namespace ShaderImpl
   {
      UiImageShader::UiImageShader(const ShaderParams &params)
          : Shader(params)
      {
         Base::ShaderInit();
      }

      void UiImageShader::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         Base::AccessAllUniformLocations(shaderProgramId);

         u_image = GetUniform("image", shaderProgramId);
         u_opacity = GetUniform("opacity", shaderProgramId);
         u_translation = GetUniform("translation", shaderProgramId);
         u_scale = GetUniform("scale", shaderProgramId);
         u_rotationRadians = GetUniform("rotationRadians", shaderProgramId);
         u_isFlipped = GetUniform("isFlipped", shaderProgramId);
         u_isCustomColor = GetUniform("isCustomColor", shaderProgramId);
         u_color = GetUniform("color", shaderProgramId);
      }

      void UiImageShader::SetImageTexture(const int32_t texSlot)
      {
         u_image.LoadUniform(texSlot);
      }

      void UiImageShader::SetIsCustomColorEnabled(const bool isCustomColorEnabled)
      {
         u_isCustomColor.LoadUniform(isCustomColorEnabled ? 1.0f : 0.0f);
      }

      void UiImageShader::SetCustomColor(const glm::vec3 &color)
      {
         u_color.LoadUniform(color);
      }

      void UiImageShader::SetOpacity(const float opacity)
      {
         u_opacity.LoadUniform(opacity);
      }

      void UiImageShader::SetTransform(const glm::vec2 &normalizedTranslation, const glm::vec2 &normalizedScale)
      {
         u_translation.LoadUniform(normalizedTranslation);
         u_scale.LoadUniform(normalizedScale);
      }

      void UiImageShader::SetRotationRadians(const float rotationRadians)
      {
         u_rotationRadians.LoadUniform(rotationRadians);
      }

      void UiImageShader::SetIsFlipped(const bool isFlipped)
      {
         u_isFlipped.LoadUniform(static_cast<int32_t>(isFlipped));
      }

      void UiImageShader::SetShaderPredefine()
      {
      }

   }
}

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
         u_opacity = GetUniform("opacity", shaderProgramId);
         u_translation = GetUniform("translation", shaderProgramId);
         u_scale = GetUniform("scale", shaderProgramId);
      }

      void UiTestShader::SetImageTexture(const int32_t texSlot)
      {
         u_image.LoadUniform(texSlot);
      }

      void UiTestShader::SetOpacity(const float opacity)
      {
         u_opacity.LoadUniform(opacity);
      }

      void UiTestShader::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
      {
         u_translation.LoadUniform(normalizedTranslation);
         u_scale.LoadUniform(normalizedScale);
      }

      void UiTestShader::SetShaderPredefine()
      {
      }

   }
}

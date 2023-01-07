#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>
#include <glm/vec2.hpp>

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {
      class UiImageShader :
         public Shader
      {
         using Base = Shader;

         Uniform u_image;
         Uniform u_translation;
         Uniform u_scale;
         Uniform u_rotationRadians;
         Uniform u_isFlipped;
         Uniform u_opacity;

      public:

         UiImageShader(const ShaderParams& params);

         void SetImageTexture(const int32_t texSlot);

         void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

         void SetRotationRadians(const float rotationRadians);

         void SetOpacity(const float opacity);

         void SetIsFlipped(const bool isFlipped);

      protected:

         void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         void SetShaderPredefine() override;

      };

   }
}


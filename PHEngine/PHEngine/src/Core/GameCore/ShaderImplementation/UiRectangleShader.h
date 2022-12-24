#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {
      class UiRectangleShader :
         public Shader
      {
         using Base = Shader;

         Uniform u_color;
         Uniform u_translation;
         Uniform u_scale;

      public:

         UiRectangleShader(const ShaderParams& params);

         void SetColor(const glm::vec4& color);

         void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


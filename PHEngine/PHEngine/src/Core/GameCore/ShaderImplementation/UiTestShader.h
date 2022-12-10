#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {
      class UiTestShader :
         public Shader
      {
         using Base = Shader;

         Uniform u_image;
         Uniform u_translation;
         Uniform u_scale;
         Uniform u_opacity;

      public:

         UiTestShader(const ShaderParams& params);

         void SetImageTexture(const int32_t texSlot);

         void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

         void SetOpacity(const float opacity);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {
      class UiTestShader :
         public Shader
      {
         using Base = Shader;

         Uniform u_color;

      public:

         UiTestShader(const ShaderParams& params);

         void SetColor(const glm::vec4& color);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


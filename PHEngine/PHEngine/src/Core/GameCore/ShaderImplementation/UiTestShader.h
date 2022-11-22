#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>
#include <glm/mat4x4.hpp>

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
         Uniform u_transformMatrix;

      public:

         UiTestShader(const ShaderParams& params);

         void SetColor(const glm::vec4& color);

         void SetTransformMatrix(const glm::mat4& transformMatrix);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


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

         Uniform u_image;
         Uniform u_transformMatrix;
         Uniform u_opacity;

      public:

         UiTestShader(const ShaderParams& params);

         void SetImageTexture(const int32_t texSlot);

         void SetTransformMatrix(const glm::mat4& transformMatrix);

         void SetOpacity(const float opacity);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


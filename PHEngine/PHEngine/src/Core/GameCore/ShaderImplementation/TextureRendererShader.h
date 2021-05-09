#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class TextureRendererShader :
         public Shader
      {

         using Base = Shader;

         Uniform u_uiTexture, u_screenSpaceMatrix, u_bPerspectiveDepthTexture, u_bSeparated;

      public:

         TextureRendererShader(const ShaderParams& params);

         void SetUiTextureSampler(int32_t uiTextureSampler);

         void SetScreenSpaceMatrix(const glm::mat4& screenSpaceMatrix);

         void SetIsDepthTexture(bool bPerspectiveDepthTexture);

         void SetIsSeparatedScreen(bool bSeparatedScreen);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {

      class CubemapShader : public Shader
      {

      private:
         Uniform u_worldMatrix, u_viewMatrix, u_projectionMatrix, u_texture;

      public:
         CubemapShader(const ShaderParams& params);

         ~CubemapShader() override;

         void SetTransformMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

         void SetTexture(int32_t texSlot);

      protected:

         void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         void SetShaderPredefine() override;
      };

   }
}


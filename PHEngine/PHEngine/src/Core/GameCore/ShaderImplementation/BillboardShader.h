#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class BillboardShader : public ShaderBase
      {

      private:
         Uniform u_worldMatrix, u_viewMatrix, u_projectionMatrix, u_texture;

      public:
         BillboardShader(const ShaderParams& params);

         virtual ~BillboardShader();

         void SetTransformMatrices(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix);

         void SetTexture(int32_t texSlot);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         virtual void SetShaderPredefine() override;
      };

   }
}


#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {

      class BillboardShader 
         : public Shader
      {

      private:
         Uniform u_billboardExtent;

      public:
         BillboardShader(const ShaderParams& params);

         ~BillboardShader() override;

         void SetExtent(const float extent);

      protected:

         void AccessAllUniformLocations(uint32_t shaderProgramID) override;
      };

   }
}


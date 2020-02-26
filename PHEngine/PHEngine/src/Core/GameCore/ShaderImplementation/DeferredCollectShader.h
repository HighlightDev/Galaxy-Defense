#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {
      class DeferredCollectShader :
         public ShaderBase
      {

         using Base = ShaderBase;

      public:

         DeferredCollectShader(const ShaderParams& params)
            : ShaderBase(params)
         {
         }

         void AccessAllUniformLocations(uint32_t shaderProgramId)
         {
            Base::AccessAllUniformLocations(shaderProgramId);
         }
      };

   }
}


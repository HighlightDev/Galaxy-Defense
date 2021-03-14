#pragma once

#include <string>
#include <stdint.h>

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "ShaderCommon.h"

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {
      class CapturePlanarReflectionShader :
         public ShaderBase
      {
         using Base = ShaderBase;

      protected:

         Uniform uClipPlane;

      public:

         CapturePlanarReflectionShader(const ShaderParams& params);

         virtual ~CapturePlanarReflectionShader();

         void SetClipPlane(const glm::vec4& plane);

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };
   }
}
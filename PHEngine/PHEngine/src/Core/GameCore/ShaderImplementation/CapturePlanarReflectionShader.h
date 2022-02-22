#pragma once

#include <string>
#include <cstddef>

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "ShaderCommon.h"

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {
      class CapturePlanarReflectionShader :
         public Shader
      {
         using Base = Shader;

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
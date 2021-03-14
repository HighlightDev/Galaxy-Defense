#include "CapturePlanarReflectionShader.h"

namespace Game
{
   namespace ShaderImpl
   {

      CapturePlanarReflectionShader::CapturePlanarReflectionShader(const ShaderParams& params)
         : ShaderBase(params)
      {
      }

      CapturePlanarReflectionShader::~CapturePlanarReflectionShader()
      {
      }

      void CapturePlanarReflectionShader::SetClipPlane(const glm::vec4& plane)
      {
         uClipPlane.LoadUniform(plane);
      }

      void CapturePlanarReflectionShader::SetShaderPredefine()
      {
      }

      void CapturePlanarReflectionShader::AccessAllUniformLocations(uint32_t shaderProgramId) {

         ShaderBase::AccessAllUniformLocations(shaderProgramId);
         uClipPlane = GetUniform("clipPlane", shaderProgramId);
      }

   }
}
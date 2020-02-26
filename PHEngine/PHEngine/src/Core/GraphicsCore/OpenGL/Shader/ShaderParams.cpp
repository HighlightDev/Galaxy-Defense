#include "ShaderParams.h"

namespace Graphics
{
   namespace OpenGL
   {
      ShaderParams::ShaderParams(const std::string& shaderName, const std::string& vertexShaderFile, const std::string& fragmentShaderFile, const std::string& geometryShaderFile,
         const std::string& tesselationControlShaderFile, const std::string& tesselationEvaluationShaderFile, const std::string& computeShaderFile)
         : ShaderName(shaderName)
         , VertexShaderFile(vertexShaderFile)
         , FragmentShaderFile(fragmentShaderFile)
         , GeometryShaderFile(geometryShaderFile)
         , TesselationControlShaderFile(tesselationControlShaderFile)
         , TesselationEvaluationShaderFile(tesselationEvaluationShaderFile)
         , ComputeShaderFile(computeShaderFile)
      {
      }

      ShaderParams::ShaderParams()
         : ShaderName("")
         , VertexShaderFile("")
         , FragmentShaderFile("")
         , GeometryShaderFile("")
         , TesselationControlShaderFile("")
         , TesselationEvaluationShaderFile("")
         , ComputeShaderFile("")
      {

      }
   }
}
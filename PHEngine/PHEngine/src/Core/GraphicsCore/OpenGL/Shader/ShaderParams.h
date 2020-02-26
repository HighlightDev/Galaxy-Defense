#pragma once

#include <string>

namespace Graphics
{
   namespace OpenGL
   {
      struct ShaderParams
      {
         std::string ShaderName;
         std::string VertexShaderFile;
         std::string FragmentShaderFile;
         std::string GeometryShaderFile;
         std::string TesselationControlShaderFile;
         std::string TesselationEvaluationShaderFile;
         std::string ComputeShaderFile;

         explicit ShaderParams(const std::string& shaderName, const std::string& vertexShaderFile, const std::string& fragmentShaderFile, const std::string& geometryShaderFile,
            const std::string& tesselationControlShaderFile, const std::string& tesselationEvaluationShaderFile, const std::string& computeShaderFile);

         explicit ShaderParams();

         friend struct std::hash<ShaderParams>;

         bool operator==(const ShaderParams& other) const {

            return this->ShaderName == other.ShaderName
               && this->VertexShaderFile == other.VertexShaderFile
               && this->FragmentShaderFile == other.FragmentShaderFile
               && this->GeometryShaderFile == other.GeometryShaderFile
               && this->TesselationControlShaderFile == other.TesselationControlShaderFile
               && this->TesselationEvaluationShaderFile == other.TesselationEvaluationShaderFile
               && this->ComputeShaderFile == other.ComputeShaderFile;
         }

      };
   }
}

namespace std
{
   using namespace Graphics::OpenGL;
   template<>
   struct hash<ShaderParams>
   {
      std::size_t operator()(const ShaderParams& k) const
      {
         return hash<std::string>()(k.ShaderName)
            ^ hash<std::string>()(k.VertexShaderFile)
            ^ hash<std::string>()(k.FragmentShaderFile)
            ^ hash<std::string>()(k.GeometryShaderFile)
            ^ hash<std::string>()(k.TesselationControlShaderFile)
            ^ hash<std::string>()(k.TesselationEvaluationShaderFile)
            ^ hash<std::string>()(k.ComputeShaderFile);
      }
   };
}



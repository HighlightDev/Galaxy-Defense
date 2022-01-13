#include "AVertexFactoryShaderModule.h"

namespace Graphics
{
   namespace OpenGL
   {
      AVertexFactoryShaderModule::AVertexFactoryShaderModule(const CompositeShaderParams& shaderParams,
         std::shared_ptr<VertexFactoryShader> vertexFactoryShader)
         : IShader(shaderParams.mShaderName)
         , mShader(shaderParams.mShader)
         , mVertexFactoryShader(vertexFactoryShader)
      {
      }

      void AVertexFactoryShaderModule::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         IShader::AccessAllUniformLocations(shaderProgramID);

         mVertexFactoryShader->AccessAllUniformLocations(shaderProgramID);
         mShader->AccessAllUniformLocations(shaderProgramID);
      }

      void AVertexFactoryShaderModule::ProcessAllPredefines()
      {
         mVertexFactoryShader->ProcessAllPredefines();
         mShader->ProcessAllPredefines();
      }

      bool AVertexFactoryShaderModule::AssembleShaderSource()
      {
         const std::string vertexFactoryShaderSource = mVertexFactoryShader->GetShaderSource();

         ShaderParams shaderParams = mShader->GetShaderParams();

         std::string vsSourcePath = shaderParams.VertexShaderFile;
         std::string fsSourcePath = shaderParams.FragmentShaderFile;
         std::string gsSourcePath = shaderParams.GeometryShaderFile;

         auto vsSource = LoadShaderSource(vsSourcePath);
         auto fsSource = LoadShaderSource(fsSourcePath);
         auto gsSource = LoadShaderSource(gsSourcePath);

         // Vertex Factory shader is combined with vertex shader
         vsSource = vertexFactoryShaderSource + "\n" + vsSource;

         // Material shader is combined with fragment shader

         ProcessShaderIncludes(vsSource);
         ProcessShaderIncludes(gsSource);
         ProcessShaderIncludes(fsSource);

         return SendToGpuShadersSources(vsSource, gsSource, fsSource);
      }

      void AVertexFactoryShaderModule::Init()
      {
         ProcessAllPredefines();

         const bool bShaderLoadedSuccessfully = AssembleShaderSource();
         if (bShaderLoadedSuccessfully)
         {
            CompileShaders();
            m_shaderProgramID = glCreateProgram();
            LinkShaders();
            AccessAllUniformLocations(m_shaderProgramID);
         }
      }

#if DEBUG

      void AVertexFactoryShaderModule::RecompileShader()
      {
         CleanUp(false);
         Init();
      }

#endif
   }
}
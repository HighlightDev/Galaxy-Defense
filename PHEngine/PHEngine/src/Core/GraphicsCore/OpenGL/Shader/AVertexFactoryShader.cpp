#include "AVertexFactoryShader.h"

namespace Graphics
{
   namespace OpenGL
   {
      AVertexFactoryShader::AVertexFactoryShader(const std::string& shaderName,
         std::shared_ptr<Shader> shader,
         std::shared_ptr<VertexFactoryShader> vertexFactoryShader)
         : IShader(shaderName)
         , mShader(shader)
         , mVertexFactoryShader(vertexFactoryShader)
      {
      }

      void AVertexFactoryShader::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         IShader::AccessAllUniformLocations(shaderProgramID);

         mVertexFactoryShader->AccessAllUniformLocations(shaderProgramID);
         mShader->AccessAllUniformLocations(shaderProgramID);
      }

      void AVertexFactoryShader::ProcessAllPredefines()
      {
         mVertexFactoryShader->ProcessAllPredefines();
         mShader->ProcessAllPredefines();
      }

      bool AVertexFactoryShader::AssembleShaderSource()
      {
         const std::string vertexFactoryShaderSource = mVertexFactoryShader->GetShaderSource();

         ShaderParams shaderParams = mShader->GetShaderParams();

         std::string vsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(shaderParams.VertexShaderFile);
         std::string fsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(shaderParams.FragmentShaderFile);
         std::string gsSourcePath = EngineUtility::ConvertFromRelativeToAbsolutePath(shaderParams.GeometryShaderFile);

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

      void AVertexFactoryShader::Init()
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

      void AVertexFactoryShader::RecompileShader()
      {
         CleanUp(false);
         Init();
      }

#endif
   }
}
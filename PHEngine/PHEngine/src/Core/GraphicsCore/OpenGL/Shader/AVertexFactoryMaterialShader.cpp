#include "AVertexFactoryMaterialShader.h"


namespace Graphics
{
   namespace OpenGL
   {
      AVertexFactoryMaterialShader::AVertexFactoryMaterialShader(const std::string& shaderName, std::shared_ptr<Shader> shader,
         std::shared_ptr<VertexFactoryShader> vertexFactoryShader,
         std::shared_ptr<MaterialShader> materialShader)
         : AVertexFactoryShader(shaderName, shader, vertexFactoryShader)
         , mMaterialShader(std::move(materialShader))
      {
      }

      AVertexFactoryMaterialShader::~AVertexFactoryMaterialShader()
      {
      }

      std::shared_ptr<MaterialShader> AVertexFactoryMaterialShader::GetMaterialShader() const
      {
         return mMaterialShader;
      }

      void AVertexFactoryMaterialShader::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         AVertexFactoryShader::AccessAllUniformLocations(shaderProgramID);
         mMaterialShader->AccessAllUniformLocations(shaderProgramID);
      }

      void AVertexFactoryMaterialShader::ProcessAllPredefines()
      {
         AVertexFactoryShader::ProcessAllPredefines();
         mMaterialShader->ProcessAllPredefines();
      }

      bool AVertexFactoryMaterialShader::AssembleShaderSource()
      {
         const std::string vertexFactoryShaderSource = mVertexFactoryShader->GetShaderSource();
         const std::string materialShaderSource = mMaterialShader->GetShaderSource();

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
         fsSource = materialShaderSource + fsSource;

         ProcessShaderIncludes(vsSource);
         ProcessShaderIncludes(gsSource);
         ProcessShaderIncludes(fsSource);

         return SendToGpuShadersSources(vsSource, gsSource, fsSource);
      }
   }
}
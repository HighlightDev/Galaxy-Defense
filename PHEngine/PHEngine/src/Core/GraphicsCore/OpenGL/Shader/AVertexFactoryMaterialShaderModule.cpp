#include "AVertexFactoryMaterialShaderModule.h"


namespace Graphics
{
   namespace OpenGL
   {
      AVertexFactoryMaterialShaderModule::AVertexFactoryMaterialShaderModule(const CompositeShaderParams& shaderParams,
         std::shared_ptr<VertexFactoryShader> vertexFactoryShader,
         std::shared_ptr<MaterialShader> materialShader)
         : AVertexFactoryShaderModule(shaderParams, vertexFactoryShader)
         , mMaterialShader(std::move(materialShader))
      {
      }

      AVertexFactoryMaterialShaderModule::~AVertexFactoryMaterialShaderModule()
      {
      }

      std::shared_ptr<MaterialShader> AVertexFactoryMaterialShaderModule::GetMaterialShader() const
      {
         return mMaterialShader;
      }

      void AVertexFactoryMaterialShaderModule::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         AVertexFactoryShaderModule::AccessAllUniformLocations(shaderProgramID);
         mMaterialShader->AccessAllUniformLocations(shaderProgramID);
      }

      void AVertexFactoryMaterialShaderModule::ProcessAllPredefines()
      {
         AVertexFactoryShaderModule::ProcessAllPredefines();
         mMaterialShader->ProcessAllPredefines();
      }

      bool AVertexFactoryMaterialShaderModule::AssembleShaderSource()
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
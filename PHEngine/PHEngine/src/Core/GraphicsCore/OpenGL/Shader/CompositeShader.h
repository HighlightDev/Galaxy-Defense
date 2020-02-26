#pragma once
#include "Shader.h"
#include "Core/CommonApi/StringHash.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/MaterialShader.h"

namespace Graphics
{
   namespace OpenGL
   {

      class ICompositeShader
      {
      public:

         ICompositeShader() {

         }

         virtual ~ICompositeShader()
         {
         }


      protected:

         std::shared_ptr<IShader> mShader;
         std::shared_ptr<VertexFactoryShader> mVertexFactoryShader;
         std::shared_ptr<IMaterialShader> mMaterialShader;

       
         ICompositeShader(std::shared_ptr<IShader> shader, std::shared_ptr<VertexFactoryShader> vertexFactoryShader, std::shared_ptr<IMaterialShader> materialShader)
            : mShader(shader)
            , mVertexFactoryShader(std::move(vertexFactoryShader))
            , mMaterialShader(std::move(materialShader))
         {
         }

      public:

         void SetMaterialShaderUniformValues() 
         {
            mMaterialShader->SetUniformValues();
         };

         std::shared_ptr<IShader> GetShader() const
         {
            return std::static_pointer_cast<IShader>(mShader);
         }

         std::shared_ptr<VertexFactoryShader> GetVertexFactoryShader() const
         {
            return std::static_pointer_cast<VertexFactoryShader>(mVertexFactoryShader);
         }

         std::shared_ptr<IMaterialShader> GetMaterialShader() const
         {
            return std::static_pointer_cast<IMaterialShader>(mMaterialShader);
         }
         
      };

      template <typename VertexFactoryShaderType, typename ShaderType>
      class CompositeShader
         : public IShader
         , public ICompositeShader
      {
      public:

         using vertexFactoryShader_t = VertexFactoryShaderType;
         using shader_t = ShaderType;

      public:

         template <typename UShader>
         CompositeShader(const std::string& compositeShaderName, const UShader& shader, std::shared_ptr<IMaterial> materialInstance)
            : IShader(compositeShaderName)
            , ICompositeShader(std::static_pointer_cast<ShaderType>(shader), std::make_shared<vertexFactoryShader_t>(), std::make_shared<MaterialShaderImp>(materialInstance))
         {
            Init();
         }

         virtual ~CompositeShader()
         {
         }

         std::shared_ptr<ShaderType> GetShader() const
         {
            return std::static_pointer_cast<ShaderType>(mShader);
         }

         std::shared_ptr<VertexFactoryShaderType> GetVertexFactoryShader() const
         {
            return std::static_pointer_cast<VertexFactoryShaderType>(mVertexFactoryShader);
         }
       
      private:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override
         {
            IShader::AccessAllUniformLocations(shaderProgramID);

            GetVertexFactoryShader()->AccessAllUniformLocations(shaderProgramID);
            GetShader()->AccessAllUniformLocations(shaderProgramID);
            GetMaterialShader()->AccessAllUniformLocations(shaderProgramID);
         }

         virtual void ProcessAllPredefines() override
         {
            GetVertexFactoryShader()->ProcessAllPredefines();
            GetShader()->ProcessAllPredefines();
            GetMaterialShader()->ProcessAllPredefines();
         }

         bool AssembleShaderSource()
         {
            const std::string vertexFactoryShaderSource = mVertexFactoryShader->GetShaderSource();
            const std::string materialShaderSource = mMaterialShader->GetShaderSource();

            ShaderParams shaderParams = GetShader()->GetShaderParams();

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

         void Init()
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
      };
   }
}


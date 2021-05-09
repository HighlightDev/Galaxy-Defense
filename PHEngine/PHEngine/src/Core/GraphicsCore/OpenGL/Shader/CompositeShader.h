#pragma once
#include "AVertexFactoryMaterialShader.h"

namespace Graphics
{
   namespace OpenGL
   {
      template <typename VertexFactoryShaderType, typename ShaderType>
      class CompositeShader
         : public AVertexFactoryMaterialShader
      {
      public:

         using vertexFactoryShader_t = VertexFactoryShaderType;
         using shader_t = ShaderType;

      public:

         CompositeShader(const std::string& compositeShaderName, std::shared_ptr<IShader> shader, std::shared_ptr<MaterialProxy> materialProxy)
            : AVertexFactoryMaterialShader(
               compositeShaderName,
               std::dynamic_pointer_cast<ShaderType>(shader),
               std::make_shared<vertexFactoryShader_t>(),
               std::make_shared<MaterialShader>(materialProxy->MaterialName,
                  materialProxy->MaterialShaderRelativePath,
                  materialProxy->GetUniformNames()))
         {
            Init();
         }

         std::shared_ptr<ShaderType> GetShader() const
         {
            return std::static_pointer_cast<ShaderType>(mShader);
         }

         std::shared_ptr<VertexFactoryShaderType> GetVertexFactoryShader() const
         {
            return std::static_pointer_cast<VertexFactoryShaderType>(mVertexFactoryShader);
         }
      };
   }
}


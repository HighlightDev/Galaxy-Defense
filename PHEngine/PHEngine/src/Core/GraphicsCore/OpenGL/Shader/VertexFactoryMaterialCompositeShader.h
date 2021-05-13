#pragma once
#include "AVertexFactoryMaterialShaderModule.h"

namespace Graphics
{
   namespace OpenGL
   {
      template <typename VertexFactoryShaderType, typename ShaderType>
      class VertexFactoryMaterialCompositeShader
         : public AVertexFactoryMaterialShaderModule
      {
      public:

         using vertexFactoryShader_t = VertexFactoryShaderType;
         using shader_t = ShaderType;

      public:

         VertexFactoryMaterialCompositeShader(const CompositeShaderParams& shaderParams)
            : AVertexFactoryMaterialShaderModule(
               shaderParams,
               std::make_shared<vertexFactoryShader_t>(),
               std::make_shared<MaterialShader>(static_cast<CompositeMaterialShaderParams&>(const_cast<CompositeShaderParams&>(shaderParams)).mMaterialProxy))
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


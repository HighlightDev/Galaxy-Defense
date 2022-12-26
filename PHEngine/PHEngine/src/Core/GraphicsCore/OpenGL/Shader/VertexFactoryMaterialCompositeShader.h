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

      protected:
         std::shared_ptr<Shader> mBaseShader;

      public:
         VertexFactoryMaterialCompositeShader(const CompositeShaderParams &compositeShaderParams)
             : AVertexFactoryMaterialShaderModule(
                   compositeShaderParams,
                   std::make_shared<VertexFactoryShaderType>(),
                   std::make_shared<MaterialShader>(static_cast<CompositeMaterialShaderParams &>(
                                                        const_cast<CompositeShaderParams &>(compositeShaderParams))
                                                        .mMaterialProxy)),
               mBaseShader(std::make_shared<ShaderType>(compositeShaderParams.mShaderParams))
         {
            Init();
         }

         std::shared_ptr<ShaderType> GetShader() const
         {
            return std::static_pointer_cast<ShaderType>(mBaseShader);
         }

         std::shared_ptr<Shader> GetBaseShader() const override
         {
            return mBaseShader;
         }

         std::shared_ptr<VertexFactoryShaderType> GetVertexFactoryShader() const
         {
            return std::static_pointer_cast<VertexFactoryShaderType>(mVertexFactoryShader);
         }
      };
   }
}

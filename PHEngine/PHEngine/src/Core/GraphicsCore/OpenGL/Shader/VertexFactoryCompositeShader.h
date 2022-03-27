#pragma once
#include "AVertexFactoryShaderModule.h"

namespace Graphics
{
   namespace OpenGL
   {
      template <typename VertexFactoryShaderType, typename ShaderType>
      class VertexFactoryCompositeShader
          : public AVertexFactoryShaderModule
      {
      protected:
         std::shared_ptr<Shader> mBaseShader;

      public:
         VertexFactoryCompositeShader(const CompositeShaderParams &compositeShaderParams)
             : AVertexFactoryShaderModule(
                   compositeShaderParams,
                   std::make_shared<VertexFactoryShaderType>()),
               mBaseShader(std::make_shared<ShaderType>(compositeShaderParams.mShaderParams))
         {
            Init();
         }

         std::shared_ptr<ShaderType> GetShader() const
         {
            return std::static_pointer_cast<ShaderType>(mBaseShader);
         }

         virtual std::shared_ptr<Shader> GetBaseShader() const override
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

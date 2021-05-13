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
      public:

         using vertexFactoryShader_t = VertexFactoryShaderType;
         using shader_t = ShaderType;

      public:

         VertexFactoryCompositeShader(const CompositeShaderParams& shaderParams)
            : AVertexFactoryShaderModule(
               shaderParams,
               std::make_shared<vertexFactoryShader_t>())
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


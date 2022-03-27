#pragma once

#include "Shader.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"

namespace Graphics
{
   namespace OpenGL
   {
      class AVertexFactoryShaderModule
          : public IShader
      {
      protected:
         std::shared_ptr<VertexFactoryShader> mVertexFactoryShader;

      public:
         AVertexFactoryShaderModule(const CompositeShaderParams &shaderParams,
                                    std::shared_ptr<VertexFactoryShader> vertexFactoryShader);

      protected:
         void Init();

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         virtual void ProcessAllPredefines() override;

         virtual bool AssembleShaderSource();

         virtual std::shared_ptr<Shader> GetBaseShader() const = 0;

#if DEBUG
         virtual void RecompileShader() override;
#endif
      };
   }
}

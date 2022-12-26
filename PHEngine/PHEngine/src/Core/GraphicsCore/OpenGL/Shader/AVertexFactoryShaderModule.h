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

         void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         void ProcessAllPredefines() override;

         virtual bool AssembleShaderSource();

         virtual std::shared_ptr<Shader> GetBaseShader() const = 0;

#if DEBUG
         void RecompileShader() override;
#endif
      };
   }
}

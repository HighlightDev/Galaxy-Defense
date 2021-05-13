#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/AVertexFactoryShaderModule.h"
#include "Core/GraphicsCore/OpenGL/Shader/MaterialShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"

namespace Graphics
{
   namespace OpenGL
   {
      class AVertexFactoryMaterialShaderModule 
         : public AVertexFactoryShaderModule
      {
      protected:

         std::shared_ptr<MaterialShader> mMaterialShader;

      public:

         AVertexFactoryMaterialShaderModule(const CompositeShaderParams& shaderParams,
            std::shared_ptr<VertexFactoryShader> vertexFactoryShader,
            std::shared_ptr<MaterialShader> materialShader);
          
         virtual ~AVertexFactoryMaterialShaderModule();

         std::shared_ptr<MaterialShader> GetMaterialShader() const;

      private:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         virtual void ProcessAllPredefines() override;

         virtual bool AssembleShaderSource() override;
      };

   }
}


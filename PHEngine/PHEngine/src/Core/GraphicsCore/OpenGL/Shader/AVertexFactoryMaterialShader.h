#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/AVertexFactoryShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/MaterialShader.h"

namespace Graphics
{
   namespace OpenGL
   {
      class AVertexFactoryMaterialShader : public AVertexFactoryShader
      {
      protected:

         std::shared_ptr<MaterialShader> mMaterialShader;

      public:

         AVertexFactoryMaterialShader(const std::string& shaderName, std::shared_ptr<Shader> shader,
            std::shared_ptr<VertexFactoryShader> vertexFactoryShader,
            std::shared_ptr<MaterialShader> materialShader);
          
         virtual ~AVertexFactoryMaterialShader();

         std::shared_ptr<MaterialShader> GetMaterialShader() const;

      private:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         virtual void ProcessAllPredefines() override;

         virtual bool AssembleShaderSource() override;
      };

   }
}


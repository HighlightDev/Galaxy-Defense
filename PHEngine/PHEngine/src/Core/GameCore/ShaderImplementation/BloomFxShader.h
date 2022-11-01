#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {
      class BloomFxShader 
         : public Shader
      {

      private:
         Uniform u_sceneColorTexture;

         uint32_t mExtractBrightPartsSubroutineIndex;

      public:
         BloomFxShader(const ShaderParams& params);

         virtual ~BloomFxShader();

         void SetSceneColorTexture(const int32_t textureSlot);

         void LoadExtractBrightPartsSubroutine();

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         virtual void AccessAllSubroutineIndices(uint32_t shaderProgramID) override;
      };

   }
}


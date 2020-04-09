#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class DefaultRenderShader
         : public ShaderBase
      {

         Uniform worldViewProjectionMatrixUniform;

      public:

         DefaultRenderShader(const ShaderParams& shaderParams)
            : ShaderBase(shaderParams)
         {
            ShaderInit();
         }

         void SetTransformMatrix(const glm::mat4& worldViewProjectionMatrix)
         {
            worldViewProjectionMatrixUniform.LoadUniform(worldViewProjectionMatrix);
         }

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override
         {
            worldViewProjectionMatrixUniform = GetUniform("worldViewProjectionMatrix", shaderProgramID);
         }
      };
   }
}
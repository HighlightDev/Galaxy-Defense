#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class DepthShaderBase
         : public ShaderBase
      {
      protected:

         Uniform u_worldMatrix, u_shadowViewMatrix, u_shadowProjectionMatrix;

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

      public:
         
         DepthShaderBase(const ShaderParams& params);

         void SetTransformationMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
      };

      template <bool IsSkeletalMesh>
      class DepthShader;

      template <>
      class DepthShader<false> :
         public DepthShaderBase
      {
         using Base = ShaderBase;

      public:

         DepthShader(const ShaderParams& params);

      protected:

         virtual void SetShaderPredefine() override;

      };

      template <>
      class DepthShader<true> :
         public DepthShaderBase
      {
         using Base = ShaderBase;

         UniformArray u_boneMatrices;

      public:

         DepthShader(const ShaderParams& params);

         void SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


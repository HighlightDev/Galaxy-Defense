#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "DepthShaderCommon.h"

#include <string>

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class DirectionalLightDepthShaderBase
         : public ShaderBase
      {
      protected:

         Uniform u_worldMatrix, u_shadowViewMatrix, u_shadowProjectionMatrix;

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

      public:
         
         DirectionalLightDepthShaderBase(const ShaderParams& params);

         void SetTransformationMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
      };

      template <eShaderMeshType type>
      class DirectionalLightDepthShader;

      template <>
      class DirectionalLightDepthShader<eShaderMeshType::NON_SKELETAL>
         : public DirectionalLightDepthShaderBase
      {
         using Base = ShaderBase;

      public:

         DirectionalLightDepthShader(const ShaderParams& params);

      protected:

         virtual void SetShaderPredefine() override;

      };

      template <>
      class DirectionalLightDepthShader<eShaderMeshType::SKELETAL>
         : public DirectionalLightDepthShaderBase
      {
         using Base = ShaderBase;

         UniformArray u_boneMatrices;

      public:

         DirectionalLightDepthShader(const ShaderParams& params);

         void SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;

      };

   }
}


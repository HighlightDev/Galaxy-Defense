#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "ShaderCommon.h"

#include <array>

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class SpotlightDepthShaderBase :
         public Shader
      {
      protected:

         Uniform u_worldMatrix, u_shadowViewMatrix, u_shadowProjectionMatrix;
         
         Uniform u_spotlightPos, u_farPlane;

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

      public:

         SpotlightDepthShaderBase(const ShaderParams& params);

         void SetTransformationMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

         void SetSpotlightPosition(const glm::vec3& position);

         void SetFarPlane(const float distance);

      };

      template <eShaderMeshType meshType>
      class SpotlightDepthShader;

      template <>
      class SpotlightDepthShader<eShaderMeshType::NON_SKELETAL>
         : public SpotlightDepthShaderBase
      {
      public:

         SpotlightDepthShader(const ShaderParams& params);

         virtual void SetShaderPredefine() override;
      };

      template <>
      class SpotlightDepthShader<eShaderMeshType::SKELETAL>
         : public SpotlightDepthShaderBase
      {
         UniformArray u_boneMatrices;

      public:

         SpotlightDepthShader(const ShaderParams& params);

         void SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;
      };
   }
}


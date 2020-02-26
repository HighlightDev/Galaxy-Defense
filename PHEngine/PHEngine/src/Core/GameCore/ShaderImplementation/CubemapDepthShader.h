#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <array>

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class CubemapDepthShaderBase :
         public ShaderBase
      {
      public:

         using six_mat4x4 = std::array<glm::mat4x4, 6>;

      protected:

         Uniform u_worldMatrix;
         UniformArray u_shadowViewMatrices, u_shadowProjectionMatrices;
         Uniform u_pointLightPos, u_farPlane;

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

      public:

         CubemapDepthShaderBase(const ShaderParams& params);

         void SetTransformationMatrices(const glm::mat4& worldMatrix, const six_mat4x4& viewMatrices, const six_mat4x4& projectionMatrices);

         void SetPointLightPosition(const glm::vec3& position);

         void SetFarPlane(const float distance);

      };

      template <bool IsSkeletalMesh>
      class CubemapDepthShader;

      template <>
      class CubemapDepthShader<false>
         : public CubemapDepthShaderBase
      {
      public:

         CubemapDepthShader(const ShaderParams& params);

         virtual void SetShaderPredefine() override;
      };

      template <>
      class CubemapDepthShader<true>
         : public CubemapDepthShaderBase
      {
         UniformArray u_boneMatrices;

      public:

         CubemapDepthShader(const ShaderParams& params);

         void SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;
      };
   }
}


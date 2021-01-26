#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "DepthShaderCommon.h"

#include <array>

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class PointLightDepthShaderBase :
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

         PointLightDepthShaderBase(const ShaderParams& params);

         void SetTransformationMatrices(const glm::mat4& worldMatrix, const six_mat4x4& viewMatrices, const six_mat4x4& projectionMatrices);

         void SetPointLightPosition(const glm::vec3& position);

         void SetFarPlane(const float distance);

      };

      template <eShaderMeshType meshType>
      class PointLightDepthShader;

      template <>
      class PointLightDepthShader<eShaderMeshType::NON_SKELETAL>
         : public PointLightDepthShaderBase
      {
      public:

         PointLightDepthShader(const ShaderParams& params);

         virtual void SetShaderPredefine() override;
      };

      template <>
      class PointLightDepthShader<eShaderMeshType::SKELETAL>
         : public PointLightDepthShaderBase
      {
         UniformArray u_boneMatrices;

      public:

         PointLightDepthShader(const ShaderParams& params);

         void SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;
      };
   }
}


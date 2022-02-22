#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "ShaderCommon.h"

#include <array>

using namespace Graphics::OpenGL;

namespace EngineCore
{
   namespace ShaderImpl
   {

      class PointLightDepthCollectShader :
         public Shader
      {
      public:

         using six_mat4x4 = std::array<glm::mat4x4, 6>;

      protected:

         UniformArray u_shadowViewMatrices, u_shadowProjectionMatrices;
         Uniform u_pointLightPos, u_farPlane;

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

      public:

         PointLightDepthCollectShader(const ShaderParams& params);

         void SetTransformationMatrices(const six_mat4x4& viewMatrices, const six_mat4x4& projectionMatrices);

         void SetPointLightPosition(const glm::vec3& position);

         void SetFarPlane(const float distance);

         virtual void SetShaderPredefine() override;

      };
   }
}


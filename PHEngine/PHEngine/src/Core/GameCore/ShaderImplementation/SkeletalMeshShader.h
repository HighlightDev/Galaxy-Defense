#pragma once

#include <string>
#include <glm/mat4x4.hpp>
#include <stdint.h>
#include <vector>

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {

      class SkeletalMeshShader :
         public ShaderBase
      {
         using Base = ShaderBase;

#define MAX_LIGHT_COUNT 1
#define MAX_WEIGHTS 3
#define MAX_BONES 55

         Uniform u_worldMatrix, u_viewMatrix, u_projectionMatrix, u_albedo, u_normalMap, u_specularMap;
         UniformArray u_boneMatrices;

      public:

         SkeletalMeshShader(const ShaderParams& params);

         virtual ~SkeletalMeshShader();

         void SetTransformationMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

         void SetAlbedoTex(int32_t slot);

         void SetNormalMapTex(int32_t slot);

         void SetSpecularMapTex(int32_t slot);

         void SetSkinningMatrices(const std::vector<glm::mat4>& matrices);

      protected:

         virtual void AccessAllUniformLocations(uint32_t shaderProgramId) override;

         virtual void SetShaderPredefine() override;
      };

   }
}
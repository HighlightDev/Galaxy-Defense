#pragma once
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <string>

using namespace Graphics::OpenGL;

namespace Game
{
   namespace ShaderImpl
   {
    
      template <bool SkeletalMeshShader>
      class DeferredShader;

      template <> 
      class DeferredShader<false> 
         : public ShaderBase
      {
         using Base = ShaderBase;

         Uniform u_worldMatrix, u_viewMatrix, u_projectionMatrix, u_albedoTex, u_normalTex, u_specularTex;

      public:

         DeferredShader(const ShaderParams& params)
            : ShaderBase(params)
         {
            ShaderInit();
         }

         void SetTransformMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
         {
            u_worldMatrix.LoadUniform(worldMatrix);
            u_viewMatrix.LoadUniform(viewMatrix);
            u_projectionMatrix.LoadUniform(projectionMatrix);
         }

         void SetAlbedoTextureSlot(int32_t slot)
         {
            u_albedoTex.LoadUniform(slot);
         }

         void SetNormalTextureSlot(int32_t slot)
         {
            u_normalTex.LoadUniform(slot);
         }

         void SetSpecularTextureSlot(int32_t slot)
         {
            u_specularTex.LoadUniform(slot);
         }

      protected:

         void AccessAllUniformLocations(uint32_t shaderProgramId)
         {
            Base::AccessAllUniformLocations(shaderProgramId);

            u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
            u_viewMatrix = GetUniform("viewMatrix", shaderProgramId);
            u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramId);
            u_albedoTex = GetUniform("albedo", shaderProgramId);
            u_normalTex = GetUniform("normalMap", shaderProgramId);
            u_specularTex = GetUniform("specularMap", shaderProgramId);
         }

         void SetShaderPredefine()
         {
         }

      };

      template <>
      class DeferredShader<true> :
         public ShaderBase
      {

         using Base = ShaderBase;

         const int32_t MaxWeights = 3;
         const int32_t MaxBones = 55;

         Uniform u_worldMatrix, u_viewMatrix, u_projectionMatrix, u_albedoTex, u_normalTex, u_specularTex;
         UniformArray u_boneMatrices;

      public:

         DeferredShader(const ShaderParams& params)
            : ShaderBase(params)
         {
            ShaderInit();
         }

         void SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices)
         {
            for (size_t index = 0; index < skinningMatrices.size(); index++)
               u_boneMatrices.LoadUniform(index, skinningMatrices[index]);
         }

         void SetTransformMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
         {
            u_worldMatrix.LoadUniform(worldMatrix);
            u_viewMatrix.LoadUniform(viewMatrix);
            u_projectionMatrix.LoadUniform(projectionMatrix);
         }

         void SetAlbedoTextureSlot(int32_t slot)
         {
            u_albedoTex.LoadUniform(slot);
         }

         void SetNormalTextureSlot(int32_t slot)
         {
            u_normalTex.LoadUniform(slot);
         }

         void SetSpecularTextureSlot(int32_t slot)
         {
            u_specularTex.LoadUniform(slot);
         }

         void AccessAllUniformLocations(uint32_t shaderProgramId)
         {
            Base::AccessAllUniformLocations(shaderProgramId);

            u_boneMatrices = GetUniformArray("bonesMatrices", MaxBones, shaderProgramId);

            u_worldMatrix = GetUniform("worldMatrix", shaderProgramId);
            u_viewMatrix = GetUniform("viewMatrix", shaderProgramId);
            u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramId);
            u_albedoTex = GetUniform("albedo", shaderProgramId);
            u_normalTex = GetUniform("normalMap", shaderProgramId);
            u_specularTex = GetUniform("specularMap", shaderProgramId);
         }

         void SetShaderPredefine()
         {
            int32_t maxWeights = MaxWeights;
            int32_t maxBones = MaxBones;
            DefineConstant<int32_t>(ShaderType::VertexShader, "MaxWeights", std::move(maxWeights));
            DefineConstant<int32_t>(ShaderType::VertexShader, "MaxBones", std::move(maxBones));
         }

      };

   }
}


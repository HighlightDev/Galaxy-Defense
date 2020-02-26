#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/CommonCore/FolderManager.h"

using namespace Graphics::OpenGL;
using namespace Common;

namespace Game
{
   template <int32_t InfluenceWeightsCount>
   class SkeletalMeshVertexFactory
      : public VertexFactoryShader
   {

      Uniform u_worldMatrix;
      Uniform u_viewMatrix;
      Uniform u_projectionMatrix;
      UniformArray u_boneMatrices;

      static constexpr int32_t MaxBones = 55;
      static constexpr int32_t MaxWeightsIndices = InfluenceWeightsCount;

  public:

     SkeletalMeshVertexFactory()
        : VertexFactoryShader("SkeletalMeshVertexFactory")
     {
        InitShader(EngineUtility::ConvertFromRelativeToAbsolutePath(FolderManager::GetInstance()->GetShadersPath() + "\\vertex_factory\\SkeletalMeshVertexFactory.glsl"));
     }

     virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override
     {
        u_worldMatrix = GetUniform("worldMatrix", shaderProgramID);
        u_viewMatrix = GetUniform("viewMatrix", shaderProgramID);
        u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
        u_boneMatrices = GetUniformArray("bonesMatrices", MaxBones, shaderProgramID);
     }

     void SetMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
     {
        u_worldMatrix.LoadUniform(worldMatrix);
        u_viewMatrix.LoadUniform(viewMatrix);
        u_projectionMatrix.LoadUniform(projectionMatrix);
     }

     void SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices)
     {
        for (size_t index = 0; index < skinningMatrices.size(); index++)
           u_boneMatrices.LoadUniform(index, skinningMatrices[index]);
     }

     virtual void SetShaderPredefine() override
     {
        DefineConstant<int32_t>("MaxBones", (int32_t)MaxBones);
        DefineConstant<int32_t>("MaxWeights", (int32_t)MaxWeightsIndices);
     }

   };
}
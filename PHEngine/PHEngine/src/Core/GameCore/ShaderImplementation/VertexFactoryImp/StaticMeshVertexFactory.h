#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/CommonCore/FolderManager.h"

using namespace Graphics::OpenGL;
using namespace Common;

namespace Game
{
   class StaticMeshVertexFactory
      : public VertexFactoryShader
   {

      Uniform u_worldMatrix;
      Uniform u_viewMatrix;
      Uniform u_projectionMatrix;

  public:

     StaticMeshVertexFactory()
        : VertexFactoryShader("StaticMeshVertexFactory")
     {
        InitShader(EngineUtility::ConvertFromRelativeToAbsolutePath(FolderManager::GetInstance()->GetShadersPath() + "\\vertex_factory\\StaticMeshVertexFactory.glsl"));
     }

     virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override
     {
        u_worldMatrix = GetUniform("worldMatrix", shaderProgramID);
        u_viewMatrix = GetUniform("viewMatrix", shaderProgramID);
        u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
     }

     void SetMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
     {
        u_worldMatrix.LoadUniform(worldMatrix);
        u_viewMatrix.LoadUniform(viewMatrix);
        u_projectionMatrix.LoadUniform(projectionMatrix);
     }
   };
}
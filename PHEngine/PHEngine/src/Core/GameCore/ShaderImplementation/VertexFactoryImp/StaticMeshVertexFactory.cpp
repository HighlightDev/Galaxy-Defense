#include "StaticMeshVertexFactory.h"
#include "Core/IoCore/FolderManager.h"

using namespace Graphics::OpenGL;
using namespace IO;

namespace EngineCore
{
   StaticMeshVertexFactory::StaticMeshVertexFactory()
       : VertexFactoryShader("StaticMeshVertexFactory")
   {
      InitShader(FolderManager::GetInstance()->GetShadersPath() + "vertex_factory" + SLASH + "StaticMeshVertexFactory.glsl");
   }

   void StaticMeshVertexFactory::AccessAllUniformLocations(uint32_t shaderProgramID)
   {
      u_worldMatrix = GetUniform("worldMatrix", shaderProgramID);
      u_viewMatrix = GetUniform("viewMatrix", shaderProgramID);
      u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
   }

   void StaticMeshVertexFactory::SetMatrices(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
   {
      u_worldMatrix.LoadUniform(worldMatrix);
      u_viewMatrix.LoadUniform(viewMatrix);
      u_projectionMatrix.LoadUniform(projectionMatrix);
   }
}
#include "StaticMeshVertexFactory.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"

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

   std::vector<std::shared_ptr<AttributeDataBase>> StaticMeshVertexFactory::GetVertexAttributes(const int32_t shaderProgramId)
   {
      std::vector<std::shared_ptr<AttributeDataBase>> result;
      result.reserve(5);
      result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexPosition>>(0));
      result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexNormal>>(1));
      result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexTexCoords>>(2));
      result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexTangent>>(3));
      result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexBitangent>>(4));
      return result;
   }
}
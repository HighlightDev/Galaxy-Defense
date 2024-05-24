#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/IoCore/FolderManager.h"

using namespace Graphics::OpenGL;
using namespace IO;

namespace EngineCore
{
   class SkyboxVertexFactory
       : public VertexFactoryShader
   {

      Uniform u_worldMatrix;
      Uniform u_viewMatrix;
      Uniform u_projectionMatrix;

   public:
      SkyboxVertexFactory()
          : VertexFactoryShader("SkyboxVertexFactory")
      {
         InitShader(FolderManager::GetInstance()->GetShadersPath() + "vertex_factory" + SLASH + "SkyboxVertexFactory.glsl");
      }

      void AccessAllUniformLocations(uint32_t shaderProgramID) override
      {
         u_worldMatrix = GetUniform("worldMatrix", shaderProgramID);
         u_viewMatrix = GetUniform("viewMatrix", shaderProgramID);
         u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
      }

      void SetMatrices(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         u_worldMatrix.LoadUniform(worldMatrix);
         u_viewMatrix.LoadUniform(viewMatrix);
         u_projectionMatrix.LoadUniform(projectionMatrix);
      }

      std::vector<std::shared_ptr<AttributeDataBase>> GetVertexAttributes(const int32_t shaderProgramId) override
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
   };
}
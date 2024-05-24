#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/IoCore/FolderManager.h"

using namespace Graphics::OpenGL;
using namespace IO;

namespace EngineCore
{
   class ParticleVertexFactory
       : public VertexFactoryShader
   {

      Uniform u_worldMatrix;
      Uniform u_viewMatrix;
      Uniform u_projectionMatrix;

   public:
      explicit ParticleVertexFactory()
          : VertexFactoryShader("ParticleVertexFactory")
      {
         InitShader(FolderManager::GetInstance()->GetShadersPath() + "vertex_factory" + SLASH + "ParticleVertexFactory.glsl");
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
         result.reserve(4);
         result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexPosition>>(0));
         result.emplace_back(std::make_shared<CustomAttributeData>("ParticleRelativeOffset", 1, eAttributeComponentDataType::FLOAT, 3));
         result.emplace_back(std::make_shared<CustomAttributeData>("ParticleRotationAndSize", 2, eAttributeComponentDataType::FLOAT, 2));
         result.emplace_back(std::make_shared<CustomAttributeData>("ParticleColor", 3, eAttributeComponentDataType::FLOAT, 4));
         return result;
      }
   };
}
#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"

using namespace Graphics::OpenGL;

namespace EngineCore
{
   class StaticMeshVertexFactory
       : public VertexFactoryShader
   {

      Uniform u_worldMatrix;
      Uniform u_viewMatrix;
      Uniform u_projectionMatrix;

   public:
      StaticMeshVertexFactory();

      void AccessAllUniformLocations(uint32_t shaderProgramID) override;

      void SetMatrices(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
   };
}
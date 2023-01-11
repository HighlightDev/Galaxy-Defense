#pragma once
#include "StaticMeshSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"

#include <glm/vec3.hpp>

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class RuntimeGeneratedLineSceneProxy : public StaticMeshSceneProxy
      {
         using Base = PrimitiveSceneProxy;
         using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;
         using PlanarReflectionShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>;

         glm::vec3 mLineBeginWorldSpacePosition;

         glm::vec3 mLineEndWorldSpacePosition;

         bool bUpdateLineGeometry{true};

      public:
         RuntimeGeneratedLineSceneProxy(const RuntimeGeneratedLineComponent *component);

         void Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

         void SetLineBeginWorldSpacePosition(const glm::vec3 &position);

         void SetLineEndWorldSpacePosition(const glm::vec3 &position);

      private:
         void UpdateGeometry(const glm::mat4& viewMatrix);
      };

   }
}

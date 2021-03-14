#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/CapturePlanarReflectionShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"

using namespace Game;
using namespace Game::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class StaticMeshSceneProxy :
         public PrimitiveSceneProxy
      {

         using Base = PrimitiveSceneProxy;
         using ShaderType = CompositeShader<StaticMeshVertexFactory, SimpleShader>;
         using PlanarReflectionShaderType = CompositeShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>;

      private:

         std::shared_ptr<ShaderType> GetShader() const;

         std::shared_ptr<PlanarReflectionShaderType> GetPlanarReflectionShader() const;

      public:
         StaticMeshSceneProxy(const StaticMeshComponent* component);

         ~StaticMeshSceneProxy();

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         virtual void RenderPlanarReflection(const glm::vec4& plane, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         virtual bool IsDeferred() const;

         virtual PrimitiveProxyType GetPrimitiveProxyType() const;
      };

   }
}


#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/StaticMeshComponent.h"
#include "Core/GameCore/ShaderImplementation/StaticMeshShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"
#include "Core/GameCore/ShaderImplementation/DeferredCollectShader.h"
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
         using ShaderType = CompositeShader<StaticMeshVertexFactory, DeferredCollectShader>;

      private:

         std::shared_ptr<ShaderType> GetShader() const;

      public:
         StaticMeshSceneProxy(const StaticMeshComponent* component);

         ~StaticMeshSceneProxy();

         virtual void Render(glm::mat4& viewMatrix, glm::mat4& projectionMatrix) override;
         virtual bool IsDeferred() const;
      };

   }
}


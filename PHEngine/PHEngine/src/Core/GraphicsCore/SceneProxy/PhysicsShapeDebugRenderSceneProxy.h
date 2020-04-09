#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/PhysicsShapeDebugRenderComponent.h"
#include "Core/GameCore/ShaderImplementation/DefaultRenderShader.h"

using namespace Game;
using namespace Game::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class PhysicsShapeDebugRenderSceneProxy :
         public PrimitiveSceneProxy
      {
         using Base = PrimitiveSceneProxy;

      private:

         std::shared_ptr<DefaultRenderShader> mShader;

      public:
         PhysicsShapeDebugRenderSceneProxy(const PhysicsShapeDebugRenderComponent* component);

         ~PhysicsShapeDebugRenderSceneProxy();

         virtual void Render(glm::mat4& viewMatrix, glm::mat4& projectionMatrix) override;

         virtual bool IsDeferred() const;
      };

   }
}


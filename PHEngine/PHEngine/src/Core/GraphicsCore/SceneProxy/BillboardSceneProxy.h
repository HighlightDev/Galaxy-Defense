#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/ShaderImplementation/BillboardShader.h"

using namespace EngineCore;
using namespace EngineCore::ShaderImpl;

namespace Graphics
{
   namespace Proxy
   {

      class BillboardSceneProxy :
         public PrimitiveSceneProxy
      {

         std::shared_ptr<BillboardShader> m_billboardShader;

         std::shared_ptr<ITexture> m_billboardTexture;

         using Base = PrimitiveSceneProxy;

      public:

         BillboardSceneProxy(const BillboardComponent* component);

         ~BillboardSceneProxy();

         virtual void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

         virtual bool IsDeferred() const override;

         virtual eMeshFacing GetMeshFrontFace() const override;
      };

   }
}


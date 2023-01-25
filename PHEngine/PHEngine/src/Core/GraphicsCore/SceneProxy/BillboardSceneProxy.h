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
      class BillboardSceneProxy : public PrimitiveSceneProxy
      {

         std::shared_ptr<BillboardShader> m_billboardShader;

         std::shared_ptr<ITexture> m_billboardTexture;

         float mBillboardExtent;

         using Base = PrimitiveSceneProxy;

      public:
         BillboardSceneProxy(const BillboardComponent *component);

         ~BillboardSceneProxy() override;

         void PostConstructorInitialize() override;

         void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;

         bool IsDeferred() const override;

         bool IsFrustumCullTestNeeded() const override;

         eMeshFacing GetMeshFrontFace() const override;

         void SetBillboardExtent(const float extent);

         void SetBillboardTexture(const std::shared_ptr<ITexture> &texture);
      };

   }
}

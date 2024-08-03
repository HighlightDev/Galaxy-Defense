#pragma once
#include "PrimitiveSceneProxy.h"
#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"

using namespace Graphics;
using namespace Graphics::Data;
using namespace EngineCore;

namespace Graphics
{
   namespace Proxy
   {

      class InstancedStaticMeshSceneProxy : public PrimitiveSceneProxy
      {
         using Base = PrimitiveSceneProxy;

      protected:
         MeshRenderData m_renderData;

      public:
         InstancedStaticMeshSceneProxy(const InstancedStaticMeshComponent *component);

         ~InstancedStaticMeshSceneProxy() override;

         void PostConstructorInitialize() override;

         void Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy,
                     const glm::mat4 &viewMatrix,
                     const glm::mat4 &projectionMatrix) override;

         void RenderPlanarReflection(const glm::vec4 &plane,
                                     const glm::mat4 &mirrorMatrix,
                                     const glm::mat4 &viewMatrix,
                                     const glm::mat4 &projectionMatrix) override;

         bool IsDeferred() const override;

         bool IsFrustumCullTestNeeded() const override;

         ePrimitiveProxyType GetPrimitiveProxyType() const override;

         MeshRenderData GetRenderData() const;

         std::string GetBatchKey() const;
      };

   }
}

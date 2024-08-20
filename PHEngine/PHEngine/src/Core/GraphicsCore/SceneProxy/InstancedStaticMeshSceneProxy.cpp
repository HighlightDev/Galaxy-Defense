#include "InstancedStaticMeshSceneProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatcher.h"
#include "Core/GraphicsCore/GeometryBatching/InstancedGeometryBatch.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;
using namespace Graphics::GeometryBatching;

namespace Graphics
{
   namespace Proxy
   {
      InstancedStaticMeshSceneProxy::InstancedStaticMeshSceneProxy(const InstancedStaticMeshComponent *component)
          : PrimitiveSceneProxy(component,
                                component->GetRenderData().mMaterialProxy),
            m_renderData(component->GetRenderData())
      {
      }

      InstancedStaticMeshSceneProxy::~InstancedStaticMeshSceneProxy()
      {
      }

      void InstancedStaticMeshSceneProxy::PostConstructorInitialize()
      {
         if (const auto &deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock())
         {
            const auto &batcherSp = deferredShadingSceneRendererSp->GetInstancedGeometryBatcher();

            if (batcherSp->CheckIfBatchExists(GetBatchKey()))
            {
               const auto &batch = batcherSp->GetBatch(GetBatchKey());
               batch->AddInstancedStaticMeshSceneProxy(shared_from_this());
            }
            else
            {
               const auto &batch = std::make_shared<InstancedGeometryBatch>(shared_from_this());
               const bool bSuccess = batcherSp->TryToAddBatch(batch);
               assert(bSuccess);
               batch->Initialize();
            }
         }
      }

      void InstancedStaticMeshSceneProxy::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         // this code should not be invoked
         assert(false);
      }

      void InstancedStaticMeshSceneProxy::RenderPlanarReflection(const glm::vec4 &plane,
                                                                 const glm::mat4 &mirrorMatrix,
                                                                 const glm::mat4 &viewMatrix,
                                                                 const glm::mat4 &projectionMatrix)
      {
         // this code should not be invoked
         assert(false);
      }

      bool InstancedStaticMeshSceneProxy::IsDeferred() const
      {
         return m_renderData.mIsDeferredShaded;
      }

      bool InstancedStaticMeshSceneProxy::IsFrustumCullTestNeeded() const
      {
         return false; // todo: right now it is not implemented, all proxies will be rendered as instanced geometry
      }

      ePrimitiveProxyType InstancedStaticMeshSceneProxy::GetPrimitiveProxyType() const
      {
         return ePrimitiveProxyType::INDIRECT_RENDERED_PROXY;
      }

      MeshRenderData InstancedStaticMeshSceneProxy::GetRenderData() const
      {
         return m_renderData;
      }

      std::string InstancedStaticMeshSceneProxy::GetBatchKey() const
      {
         return m_renderData.mModelPath + "_" + m_renderData.mMaterialProxy->MaterialName;
      }

   }
}

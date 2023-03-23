#include "BillboardSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics
{
   namespace Proxy
   {

      BillboardSceneProxy::BillboardSceneProxy(const BillboardComponent *component)
          : PrimitiveSceneProxy(component,
                                nullptr,
                                component->GetRenderData().m_shader,
                                nullptr,
                                component->GetRenderData().mMaterialProxy),
            mRenderData(component->GetRenderData()),
            mBillboardExtent(component->GetBillboardExtent())
      {
      }

      BillboardSceneProxy::~BillboardSceneProxy()
      {
      }

      void BillboardSceneProxy::PostConstructorInitialize()
      {
         static constexpr uint64_t functionId = Hash64_CT("BillboardSceneProxy::PostConstructorInitialize");
         m_skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource((int32_t)SimplePrimitiveType::POINT);

         if (const auto &deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock())
         {
            if (const auto &sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock())
            {
               const auto boundingBox = m_skin->GetBoundingBox();
               sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mSceneProxyId, functionId,
                                            [this, sceneSp, boundingBox]()
                                            {
                                               const auto &engineObject = sceneSp->GetEngineObjectById(GetGameObjectId());
                                               assert(engineObject);
                                               const auto &primitiveComponent = static_cast<PrimitiveComponent *>(engineObject);
                                               assert(primitiveComponent);
                                               primitiveComponent->SetBoundingBox(boundingBox);
                                            });
            }
         }
      }

      std::shared_ptr<typename BillboardSceneProxy::Shader_t> BillboardSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<typename BillboardSceneProxy::Shader_t>(m_shader);
      }

      void BillboardSceneProxy::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         const auto& billboardShader = GetShader();

         billboardShader->ExecuteShader();
         billboardShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         billboardShader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         billboardShader->GetShader()->SetExtent(mBillboardExtent);
         m_skin->GetBuffer()->RenderVAO(GL_POINTS);
         billboardShader->StopShader();
      }

      bool BillboardSceneProxy::IsDeferred() const
      {
         return false;
      }

      bool BillboardSceneProxy::IsFrustumCullTestNeeded() const
      {
         return false;
      }

      eMeshFacing BillboardSceneProxy::GetMeshFrontFace() const
      {
         return eMeshFacing::COUNTER_CLOCK_WISE;
      }

      void BillboardSceneProxy::SetBillboardExtent(const float extent)
      {
         mBillboardExtent = extent;
      }

   }
}

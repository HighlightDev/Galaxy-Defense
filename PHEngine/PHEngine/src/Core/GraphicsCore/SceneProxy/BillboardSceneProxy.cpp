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
          : PrimitiveSceneProxy(component->IsEnabled(),
                                component->IsVisible(),
                                component->GetRelativeMatrix(),
                                nullptr,
                                nullptr,
                                nullptr,
                                nullptr),
            m_billboardShader(std::static_pointer_cast<BillboardShader>(component->GetRenderData().m_shader)),
            m_billboardTexture(component->GetRenderData().m_texture),
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
            if (const auto &sceneSp = deferredShadingSceneRendererSp->GetThreadManager().GetSceneWP().lock())
            {
               const auto boundingBox = m_skin->GetBoundingBox();
               sceneSp->ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mSceneProxyId, functionId,
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

      void BillboardSceneProxy::Render(const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         auto billboardShader = m_billboardShader;

         billboardShader->ExecuteShader();

         m_billboardTexture->BindTexture(0);

         billboardShader->SetTexture(0);
         billboardShader->SetExtent(mBillboardExtent);
         billboardShader->SetTransformMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);

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

      void BillboardSceneProxy::SetBillboardTexture(const std::shared_ptr<ITexture> &texture)
      {
         if (m_billboardTexture->GetTextureDescriptor() != texture->GetTextureDescriptor())
         {
            m_billboardTexture = texture;
         }
      }

   }
}

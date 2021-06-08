#include "BillboardSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      BillboardSceneProxy::BillboardSceneProxy(const BillboardComponent* component)
         : PrimitiveSceneProxy(component->IsVisible(), component->GetRelativeMatrix(), component->GetRenderData().m_skin, nullptr, nullptr, nullptr)
         , m_billboardShader(std::static_pointer_cast<BillboardShader>(component->GetRenderData().m_shader))
         , m_billboardTexture(component->GetRenderData().m_texture)
      {
      }

      BillboardSceneProxy::~BillboardSceneProxy()
      {
      }

      void BillboardSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         auto billboardShader = m_billboardShader;

         billboardShader->ExecuteShader();

         m_billboardTexture->BindTexture(0);

         billboardShader->SetTexture(0);
         billboardShader->SetTransformMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);

         m_skin->GetBuffer()->RenderVAO(GL_POINTS);
         billboardShader->StopShader();
      }

      bool BillboardSceneProxy::IsDeferred() const 
      {
         return false;
      }

      eMeshFacing BillboardSceneProxy::GetMeshFrontFace() const {
         return eMeshFacing::COUNTER_CLOCK_WISE;
      }

   }
}

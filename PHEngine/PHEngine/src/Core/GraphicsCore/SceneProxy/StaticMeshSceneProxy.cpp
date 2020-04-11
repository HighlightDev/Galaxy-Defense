#include "StaticMeshSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      StaticMeshSceneProxy::StaticMeshSceneProxy(const StaticMeshComponent* component)
         : PrimitiveSceneProxy(component->GetRelativeMatrix(), component->GetRenderData().m_skin, component->GetRenderData().m_materialShader, component->GetRenderData().mMaterialInstance)
      {
      }

      StaticMeshSceneProxy::~StaticMeshSceneProxy()
      {
      }

      std::shared_ptr<StaticMeshSceneProxy::ShaderType> StaticMeshSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<StaticMeshSceneProxy::ShaderType>(m_shader);
      }

      void StaticMeshSceneProxy::Render(glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
      {
         const auto& shader = GetShader();

         shader->ExecuteShader();
         shader->GetMaterialShader()->SetUniformValues(mMaterialInstance);
         shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shader->StopShader();
      }

      bool StaticMeshSceneProxy::IsDeferred() const
      {
         return true;
      }

   }
}

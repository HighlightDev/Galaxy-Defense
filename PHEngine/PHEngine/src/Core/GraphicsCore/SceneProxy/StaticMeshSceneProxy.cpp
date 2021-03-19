#include "StaticMeshSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      StaticMeshSceneProxy::StaticMeshSceneProxy(const StaticMeshComponent* component)
         : PrimitiveSceneProxy(component->IsVisible()
            , component->GetRelativeMatrix()
            , component->GetRenderData().m_skin
            , component->GetRenderData().m_materialShader
            , component->GetRenderData().m_planarReflectionShader
            , component->GetRenderData().mMaterialProxy)
      {
      }

      StaticMeshSceneProxy::~StaticMeshSceneProxy()
      {
      }

      std::shared_ptr<StaticMeshSceneProxy::ShaderType> StaticMeshSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<StaticMeshSceneProxy::ShaderType>(m_shader);
      }

      std::shared_ptr<StaticMeshSceneProxy::PlanarReflectionShaderType> StaticMeshSceneProxy::GetPlanarReflectionShader() const
      {
         return std::static_pointer_cast<StaticMeshSceneProxy::PlanarReflectionShaderType>(m_planarReflectionShader);
      }

      void StaticMeshSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         const auto& shader = GetShader();

         shader->ExecuteShader();
         shader->GetMaterialShader()->LoadUniformValues();
         shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shader->StopShader();
      }

      void StaticMeshSceneProxy::RenderPlanarReflection(const glm::vec4& plane, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         const auto& planarReflectionShader = GetPlanarReflectionShader();

         const glm::mat4& mirrorMatrix = glm::mat4(
            glm::vec4(-2.f*plane.x*plane.x + 1.f, -2.f*plane.y*plane.x, -2.f*plane.z*plane.x, 0.f),
            glm::vec4(-2.f*plane.x*plane.y, -2.f*plane.y*plane.y + 1.f, -2.f*plane.z*plane.y, 0.f),
            glm::vec4(-2.f*plane.x*plane.z, -2.f*plane.y*plane.z, -2.f*plane.z*plane.z + 1.f, 0.f),
            glm::vec4(2.f*plane.x*plane.w, 2.f*plane.y*plane.w, 2.f*plane.z*plane.w, 1.f));

         planarReflectionShader->ExecuteShader();
         planarReflectionShader->GetShader()->SetClipPlane(plane);
         planarReflectionShader->GetMaterialShader()->LoadUniformValues();
         planarReflectionShader->GetVertexFactoryShader()->SetMatrices(mirrorMatrix * m_relativeMatrix, viewMatrix, projectionMatrix);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         planarReflectionShader->StopShader();
      }

      bool StaticMeshSceneProxy::IsDeferred() const
      {
         return true;
      }

      PrimitiveProxyType StaticMeshSceneProxy::GetPrimitiveProxyType() const
      {
         return PrimitiveProxyType::STATIC_MESH_PROXY;
      }

   }
}

#include "StaticMeshSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      StaticMeshSceneProxy::StaticMeshSceneProxy(const StaticMeshComponent *component)
          : PrimitiveSceneProxy(
                component->IsEnabled(), component->IsVisible(),
                component->GetRelativeMatrix(),
                component->GetRenderData().m_skin,
                component->GetRenderData().m_materialShader,
                component->GetRenderData().m_planarReflectionShader,
                component->GetRenderData().mMaterialProxy)
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

      void StaticMeshSceneProxy::Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         const auto &shader = GetShader();

         shader->ExecuteShader();
         shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shader->StopShader();
      }

      void StaticMeshSceneProxy::RenderPlanarReflection(const glm::vec4 &plane, const glm::mat4 &mirrorMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         const auto &planarReflectionShader = GetPlanarReflectionShader();

         planarReflectionShader->ExecuteShader();
         planarReflectionShader->GetShader()->SetClipPlane(plane);
         planarReflectionShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         planarReflectionShader->GetVertexFactoryShader()->SetMatrices(mirrorMatrix * m_relativeMatrix, viewMatrix, projectionMatrix);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         planarReflectionShader->StopShader();
      }

      bool StaticMeshSceneProxy::IsDeferred() const
      {
         return true;
      }

      eMeshFacing StaticMeshSceneProxy::GetMeshFrontFace() const
      {
         return eMeshFacing::COUNTER_CLOCK_WISE;
      }

      ePrimitiveProxyType StaticMeshSceneProxy::GetPrimitiveProxyType() const
      {
         return ePrimitiveProxyType::STATIC_MESH_PROXY;
      }

   }
}

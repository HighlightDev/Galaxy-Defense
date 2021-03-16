#include "SkyboxSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      SkyboxSceneProxy::SkyboxSceneProxy(const SkyboxComponent* component)
         : PrimitiveSceneProxy(component->IsVisible()
            , component->GetRelativeMatrix(), component->GetRenderData().m_skin
            , component->GetRenderData().m_materialShader
            , component->GetRenderData().m_planarReflectionShader
            , component->GetRenderData().mMaterialProxy)
      {
      }

      SkyboxSceneProxy::~SkyboxSceneProxy()
      {
      }

      void SkyboxSceneProxy::PostConstructorInitialize()
      {

      }

      std::shared_ptr<SkyboxSceneProxy::ShaderType> SkyboxSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<SkyboxSceneProxy::ShaderType>(m_shader);
      }

      std::shared_ptr<SkyboxSceneProxy::PlanarReflectionShaderType> SkyboxSceneProxy::GetPlanarReflectionShader() const
      {
         return std::static_pointer_cast<SkyboxSceneProxy::PlanarReflectionShaderType>(m_planarReflectionShader);
      }

      PrimitiveProxyType SkyboxSceneProxy::GetPrimitiveProxyType() const
      {
         return PrimitiveProxyType::PRIMITIVE_PROXY;
      }

      void SkyboxSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         glEnable(GL_CULL_FACE);
         glCullFace(GL_FRONT);

         glm::mat4 viewMatrixNoTranslation = viewMatrix;
         viewMatrixNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, viewMatrixNoTranslation[3].w);

         auto shaderPtr = GetShader();

         shaderPtr->ExecuteShader();
         shaderPtr->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrixNoTranslation, projectionMatrix);
         shaderPtr->GetMaterialShader()->SetUniformValues(mMaterialProxy);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shaderPtr->StopShader();
      }

      void SkyboxSceneProxy::RenderPlanarReflection(const glm::vec4& plane, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         glEnable(GL_CULL_FACE);
         glCullFace(GL_BACK);
         const auto& planarReflectionShader = GetPlanarReflectionShader();

         const glm::mat4& mirrorMatrix = glm::mat4(
            glm::vec4(-2.f*plane.x*plane.x + 1.f, -2.f*plane.y*plane.x, -2.f*plane.z*plane.x, 0.f),
            glm::vec4(-2.f*plane.x*plane.y, -2.f*plane.y*plane.y + 1.f, -2.f*plane.z*plane.y, 0.f),
            glm::vec4(-2.f*plane.x*plane.z, -2.f*plane.y*plane.z, -2.f*plane.z*plane.z + 1.f, 0.f),
            glm::vec4(2.f*plane.x*plane.w, 2.f*plane.y*plane.w, 2.f*plane.z*plane.w, 1.f));

         glm::mat4 viewMatrixNoTranslation = viewMatrix;
         viewMatrixNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, viewMatrixNoTranslation[3].w);

         planarReflectionShader->ExecuteShader();
         planarReflectionShader->GetShader()->SetClipPlane(plane);
         planarReflectionShader->GetVertexFactoryShader()->SetMatrices(mirrorMatrix * m_relativeMatrix, viewMatrixNoTranslation, projectionMatrix);
         planarReflectionShader->GetMaterialShader()->SetUniformValues(mMaterialProxy);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         planarReflectionShader->StopShader();
      }

      bool SkyboxSceneProxy::IsFrustumCullTestNeeded() const
      {
         return false;
      }

      bool SkyboxSceneProxy::IsDeferred() const
      {
         return false;
      }
   }
}
#include "SkyboxSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      SkyboxSceneProxy::SkyboxSceneProxy(const SkyboxComponent* component)
         : PrimitiveSceneProxy(component->IsVisible()
            , component->GetRelativeMatrix()
            , component->GetRenderData().m_skin
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
         shaderPtr->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shaderPtr->StopShader();
      }

      void SkyboxSceneProxy::RenderPlanarReflection(const glm::vec4& plane, const glm::mat4& mirrorMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         glEnable(GL_CULL_FACE);
         glCullFace(GL_BACK);
         const auto& planarReflectionShader = GetPlanarReflectionShader();

         glm::mat4 viewMatrixNoTranslation = viewMatrix;
         viewMatrixNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, viewMatrixNoTranslation[3].w);

         planarReflectionShader->ExecuteShader();
         planarReflectionShader->GetShader()->SetClipPlane(plane);
         planarReflectionShader->GetVertexFactoryShader()->SetMatrices(mirrorMatrix * m_relativeMatrix, viewMatrixNoTranslation, projectionMatrix);
         planarReflectionShader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
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
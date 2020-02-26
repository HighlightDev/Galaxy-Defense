#include "SkyboxSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      SkyboxSceneProxy::SkyboxSceneProxy(const SkyboxComponent* component)
         : PrimitiveSceneProxy(component->GetRelativeMatrix(), component->GetRenderData().m_skin, nullptr)
         , m_skyboxShader(std::static_pointer_cast<SkyboxShader>(component->GetRenderData().m_shader))
         , m_dayTexture(component->GetRenderData().m_dayTex)
         , m_nightTexture(component->GetRenderData().m_nightTex)
      {
      }

      SkyboxSceneProxy::~SkyboxSceneProxy()
      {
      }

      void SkyboxSceneProxy::PostConstructorInitialize()
      {

      }

      uint64_t SkyboxSceneProxy::GetComponentType() const
      {
         return SKYBOX_COMPONENT;
      }

      void SkyboxSceneProxy::Render(glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
      {
         glDisable(GL_CLIP_DISTANCE0);
         glEnable(GL_CULL_FACE);
         glCullFace(GL_FRONT);

         glm::mat4 viewMatrixNoTranslation = viewMatrix;
         viewMatrixNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, viewMatrixNoTranslation[3].w);

         m_skyboxShader->ExecuteShader();

         if (m_dayTexture)
         {
            m_dayTexture->BindTexture(0);
         }

         if (m_nightTexture)
         {
            m_nightTexture->BindTexture(1);
         }
         m_skyboxShader->SetTransformMatrices(m_relativeMatrix, viewMatrixNoTranslation, projectionMatrix);
         m_skyboxShader->SetTextures(0, 1);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         m_skyboxShader->StopShader();

         glDisable(GL_CULL_FACE);
      }

      bool SkyboxSceneProxy::IsDeferred() const {
         return false;
      }
   }
}
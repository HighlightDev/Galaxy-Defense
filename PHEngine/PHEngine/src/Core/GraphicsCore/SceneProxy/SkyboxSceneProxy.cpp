#include "SkyboxSceneProxy.h"

namespace Graphics
{
   namespace Proxy
   {

      SkyboxSceneProxy::SkyboxSceneProxy(const SkyboxComponent* component)
         : PrimitiveSceneProxy(component->GetRelativeMatrix(), component->GetRenderData().m_skin,
            component->GetRenderData().m_materialShader, component->GetRenderData().mMaterialInstance)
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

      uint64_t SkyboxSceneProxy::GetComponentType() const
      {
         return SKYBOX_COMPONENT;
      }

      void SkyboxSceneProxy::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
      {
         glDisable(GL_CLIP_DISTANCE0);
         glEnable(GL_CULL_FACE);
         glCullFace(GL_FRONT);

         glm::mat4 viewMatrixNoTranslation = viewMatrix;
         viewMatrixNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, viewMatrixNoTranslation[3].w);

         auto shaderPtr = GetShader();

         shaderPtr->ExecuteShader();
         shaderPtr->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrixNoTranslation, projectionMatrix);
         shaderPtr->GetMaterialShader()->SetUniformValues(mMaterialInstance);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shaderPtr->StopShader();

         glDisable(GL_CULL_FACE);
      }

      bool SkyboxSceneProxy::IsDeferred() const {

         return false;
      }
   }
}
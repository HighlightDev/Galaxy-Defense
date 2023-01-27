#include "SkyboxSceneProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics
{
   namespace Proxy
   {
      SkyboxSceneProxy::SkyboxSceneProxy(const SkyboxComponent *component)
          : PrimitiveSceneProxy(component,
                                nullptr,
                                component->GetRenderData().m_materialShader,
                                component->GetRenderData().m_planarReflectionShader,
                                component->GetRenderData().mMaterialProxy)
      {
      }

      SkyboxSceneProxy::~SkyboxSceneProxy()
      {
      }

      void SkyboxSceneProxy::PostConstructorInitialize()
      {
         static constexpr uint64_t functionId = Hash64_CT("SkyboxSceneProxy::PostConstructorInitialize");
         m_skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(static_cast<int32_t>(SimplePrimitiveType::INVERTED_VERTICES_DIRECTION_CUBE));

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

      std::shared_ptr<SkyboxSceneProxy::ShaderType> SkyboxSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<SkyboxSceneProxy::ShaderType>(m_shader);
      }

      std::shared_ptr<SkyboxSceneProxy::PlanarReflectionShaderType> SkyboxSceneProxy::GetPlanarReflectionShader() const
      {
         return std::static_pointer_cast<SkyboxSceneProxy::PlanarReflectionShaderType>(m_planarReflectionShader);
      }

      ePrimitiveProxyType SkyboxSceneProxy::GetPrimitiveProxyType() const
      {
         return ePrimitiveProxyType::PRIMITIVE_PROXY;
      }

      void SkyboxSceneProxy::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         glm::mat4 viewMatrixNoTranslation = viewMatrix;
         viewMatrixNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, viewMatrixNoTranslation[3].w);

         auto shaderPtr = GetShader();

         shaderPtr->ExecuteShader();
         shaderPtr->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrixNoTranslation, projectionMatrix);
         shaderPtr->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shaderPtr->StopShader();
      }

      void SkyboxSceneProxy::RenderPlanarReflection(const glm::vec4 &plane, const glm::mat4 &mirrorMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         const auto &planarReflectionShader = GetPlanarReflectionShader();

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

      eMeshFacing SkyboxSceneProxy::GetMeshFrontFace() const
      {
         return eMeshFacing::COUNTER_CLOCK_WISE;
      }
   }
}
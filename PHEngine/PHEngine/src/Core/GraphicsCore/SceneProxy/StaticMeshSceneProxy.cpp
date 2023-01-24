#include "StaticMeshSceneProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics
{
   namespace Proxy
   {
      StaticMeshSceneProxy::StaticMeshSceneProxy(const StaticMeshComponent *component)
          : PrimitiveSceneProxy(
                component->IsEnabled(), component->IsVisible(),
                component->GetRelativeMatrix(),
                nullptr,
                component->GetRenderData().m_materialShader,
                component->GetRenderData().m_planarReflectionShader,
                component->GetRenderData().mMaterialProxy),
            m_renderData(component->GetRenderData()),
            mIsDeferredShaded(component->GetRenderData().mIsDeferredShaded)
      {
      }

      StaticMeshSceneProxy::~StaticMeshSceneProxy()
      {
      }

      void StaticMeshSceneProxy::PostConstructorInitialize()
      {
         static constexpr uint64_t functionId = Hash64_CT("StaticMeshSceneProxy::PostConstructorInitialize");
         m_skin = MeshPool::GetInstance()->GetOrAllocateResource(m_renderData.mModelPath);

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
         return mIsDeferredShaded;
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

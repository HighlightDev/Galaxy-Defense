#include "ForwardShadingMeshSceneProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/MeshPool.h"

#include <glm/vec3.hpp>

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics
{
   namespace Proxy
   {
      ForwardShadingMeshSceneProxy::ForwardShadingMeshSceneProxy(const ForwardShadingMeshComponent *component)
          : PrimitiveSceneProxy(component,
                                component->GetRenderData().mMaterialProxy),
            mRenderData(component->GetRenderData())
      {
      }

      void ForwardShadingMeshSceneProxy::PostConstructorInitialize()
      {
         static constexpr uint64_t functionId = Hash64_CT("ForwardShadingMeshSceneProxy::PostConstructorInitialize");
         m_skin = MeshPool::GetInstance()->GetOrAllocateResource(mRenderData.mModelPath);

         const ShaderParams shaderParams(
             "ForwardWaterPlane Shader",
             FolderManager::GetInstance()->GetShadersPath() +
                 "composite_shaders" + SLASH + "simpleVS.glsl",
             FolderManager::GetInstance()->GetShadersPath() +
                 "composite_shaders" + SLASH + "forwardFS.glsl");

         m_shader = CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
             "StaticMeshVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);

         if (const auto &deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock())
         {
            if (const auto &sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock())
            {
               const auto boundingBox = m_skin->GetBoundingBox();
               sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mSceneProxyId, functionId,
                                                                                 [this, sceneSp, boundingBox]()
                                                                                 {
                                                                                    const auto &engineObject = sceneSp->GetEngineObjectById(GetGameObjectId());
                                                                                    assert(engineObject);
                                                                                    const auto &primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                                                                                    assert(primitiveComponent);
                                                                                    primitiveComponent->SetBoundingBox(boundingBox);
                                                                                 });
            }
         }
      }

      std::shared_ptr<ForwardShadingMeshSceneProxy::ShaderType> ForwardShadingMeshSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<ForwardShadingMeshSceneProxy::ShaderType>(m_shader);
      }

      ePrimitiveProxyType ForwardShadingMeshSceneProxy::GetPrimitiveProxyType() const
      {
         return ePrimitiveProxyType::PRIMITIVE_PROXY;
      }

      eMeshFacing ForwardShadingMeshSceneProxy::GetMeshFrontFace() const
      {
         return eMeshFacing::COUNTER_CLOCK_WISE;
      }

      bool ForwardShadingMeshSceneProxy::IsDeferred() const
      {
         return false;
      }

      void ForwardShadingMeshSceneProxy::Render(const std::shared_ptr<CameraSceneProxy> &cameraSceneProxy, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
      {
         const auto &shader = GetShader();

         shader->ExecuteShader();
         shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy);
         shader->GetVertexFactoryShader()->SetMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
         m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
         shader->StopShader();
      }
   }
}

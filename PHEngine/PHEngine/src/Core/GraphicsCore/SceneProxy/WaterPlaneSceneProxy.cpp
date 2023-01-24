#include "WaterPlaneSceneProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

#include <glm/vec3.hpp>

using namespace Graphics::Renderer;
using namespace EngineCore;
using namespace Resources;

namespace Graphics
{
   namespace Proxy
   {
      WaterPlaneSceneProxy::WaterPlaneSceneProxy(const WaterPlaneComponent *component)
          : PrimitiveSceneProxy(
                component->IsEnabled(),
                component->IsVisible(),
                component->GetRelativeMatrix(),
                nullptr,
                component->GetRenderData().m_materialShader,
                nullptr,
                component->GetRenderData().mMaterialProxy),
            m_moveFactor(component->GetMoveFactor()),
            m_waveStrength(component->GetWaveStrength()),
            m_transparencyDepth(component->GetTransparencyDepth()),
            m_nearClipPlane(component->GetNearClipPlane()),
            m_farClipPlane(component->GetFarClipPlane())
      {
         Init();
      }

      void WaterPlaneSceneProxy::PostConstructorInitialize()
      {
         static constexpr uint64_t functionId = Hash64_CT("WaterPlaneSceneProxy::PostConstructorInitialize");
         m_skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource((int32_t)SimplePrimitiveType::PLANE_WITH_ATTRIBUTES);

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

      std::shared_ptr<WaterPlaneSceneProxy::ShaderType> WaterPlaneSceneProxy::GetShader() const
      {
         return std::static_pointer_cast<WaterPlaneSceneProxy::ShaderType>(m_shader);
      }

      WaterPlaneSceneProxy::~WaterPlaneSceneProxy()
      {
      }

      void WaterPlaneSceneProxy::Init()
      {
      }

      void WaterPlaneSceneProxy::SetMoveFactor(float moveFactor)
      {
         m_moveFactor = moveFactor;
      }

      void WaterPlaneSceneProxy::SetWaveStrength(float waveStr)
      {
         m_waveStrength = waveStr;
      }

      void WaterPlaneSceneProxy::SetTransparencyDepth(float transparencyDepth)
      {
         m_transparencyDepth = transparencyDepth;
      }

      void WaterPlaneSceneProxy::SetNearClipPlane(float nearClipPlane)
      {
         m_nearClipPlane = nearClipPlane;
      }

      void WaterPlaneSceneProxy::SetFarClipPlane(float farClipPlane)
      {
         m_farClipPlane = farClipPlane;
      }

      ePrimitiveProxyType WaterPlaneSceneProxy::GetPrimitiveProxyType() const
      {
         return ePrimitiveProxyType::PRIMITIVE_PROXY;
      }

      eMeshFacing WaterPlaneSceneProxy::GetMeshFrontFace() const
      {
         return eMeshFacing::COUNTER_CLOCK_WISE;
      }

      bool WaterPlaneSceneProxy::IsDeferred() const
      {
         return false;
      }

      void WaterPlaneSceneProxy::Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
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

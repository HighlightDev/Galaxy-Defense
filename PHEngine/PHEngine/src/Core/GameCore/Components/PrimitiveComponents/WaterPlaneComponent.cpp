#include "WaterPlaneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneProxy/WaterPlaneSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/CommonCore/StringHash.h"

#include <glm/vec3.hpp>

using namespace Graphics::Renderer;

namespace Game
{

   WaterPlaneComponent::WaterPlaneComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, WaterPlaneRenderData&& renderData, WaterQualityFlag waterQuality)
      : PrimitiveComponent(gameObjectName, translation, rotation, scale, renderData.m_skin->GetBoundingBox())
      , m_waveSpeed(0.4f)
      , m_moveFactor(0.0f)
      , m_renderData(std::forward<WaterPlaneRenderData>(renderData))
      , m_waterQuality(waterQuality)
   {
   }

   WaterPlaneComponent::~WaterPlaneComponent()
   {
   }
   
   ComponentType WaterPlaneComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void WaterPlaneComponent::Tick(const float deltaTime)
   {
      Base::Tick(deltaTime);

      m_moveFactor += m_waveSpeed * deltaTime;

      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetMoveFactor");

            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]()
            {
               WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(sceneRenderer->SceneProxies[SceneProxyId].get());
               proxyPtr->SetMoveFactor(m_moveFactor);
            });
         }
      }
   }

   float WaterPlaneComponent::GetMoveFactor() const
   {
      return m_moveFactor;
   }

   float WaterPlaneComponent::GetWaveStrength() const
   {
      return m_waveStrength;
   }

   float WaterPlaneComponent::GetTransparencyDepth() const {

      return m_transparencyDepth;
   }

   void WaterPlaneComponent::SetWaveStrength(float waveStr)
   {
      m_waveStrength = waveStr;

      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetWaveStrength");

            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

               WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(sceneRenderer->SceneProxies[SceneProxyId].get());
               proxyPtr->SetWaveStrength(m_waveStrength);
            });
         }
      }
   }

   void WaterPlaneComponent::SetTransparencyDepth(float transparencyDepth)
   {
      m_transparencyDepth = transparencyDepth;

      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetTransparencyDepth");

            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

               WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(sceneRenderer->SceneProxies[SceneProxyId].get());
               proxyPtr->SetTransparencyDepth(m_transparencyDepth);
            });
         }
      }
   }

   std::shared_ptr<PrimitiveSceneProxy> WaterPlaneComponent::CreateSceneProxy() const
   {
      return std::make_shared<WaterPlaneSceneProxy>(this);
   }

   float WaterPlaneComponent::GetNearClipPlane() const 
   {
      return m_nearClipPlane;
   }

   float WaterPlaneComponent::GetFarClipPlane() const
   {
      return m_farClipPlane;
   }

   void WaterPlaneComponent::SetNearClipPlane(float nearClipPlane)
   {
      m_nearClipPlane = nearClipPlane;

      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetNearClipPlane");

            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, functionId, GetObjectId(), [=]() {

               WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(sceneRenderer->SceneProxies[SceneProxyId].get());
               proxyPtr->SetNearClipPlane(m_nearClipPlane);
            });
         }
      }
   }

   void WaterPlaneComponent::SetFarClipPlane(float farClipPlane)
   {
      m_farClipPlane = farClipPlane;

      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetFarClipPlane");

            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, functionId, GetObjectId(), [=]() {

               WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(sceneRenderer->SceneProxies[SceneProxyId].get());
               proxyPtr->SetFarClipPlane(m_farClipPlane);
            });
         }
      }
   }

}

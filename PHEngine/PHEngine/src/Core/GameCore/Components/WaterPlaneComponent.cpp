#include "WaterPlaneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneProxy/WaterPlaneSceneProxy.h"
#include "Core/CommonApi/StringHash.h"

#include <glm/vec3.hpp>

namespace Game
{

   WaterPlaneComponent::WaterPlaneComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, WaterPlaneRenderData&& renderData, WaterQualityFlag waterQuality)
      : PrimitiveComponent(translation, rotation, scale)
      , m_waveSpeed(0.4f)
      , m_moveFactor(0.0f)
      , m_renderData(std::forward<WaterPlaneRenderData>(renderData))
      , m_waterQuality(waterQuality)
   {
   }

   WaterPlaneComponent::~WaterPlaneComponent()
   {
   }
   
   uint64_t WaterPlaneComponent::GetComponentType() const
   {
      return WATER_PLANE_COMPONENT;
   }

   int sign = 1;

   void WaterPlaneComponent::Tick(const float deltaTime)
   {
      Base::Tick(deltaTime);

      m_moveFactor += sign * m_waveSpeed * deltaTime;

      if (m_moveFactor > 1.0f)
      {
         //m_moveFactor -= 1.0f;
         sign = -1;
      }
      else if (m_moveFactor < 0.0f)
      {
         sign = 1;
      }

      constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetMoveFactor");

      m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

         WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(m_scene->SceneProxies[PrimitiveProxyComponentId].get());
         proxyPtr->SetMoveFactor(m_moveFactor);
      });
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

      constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetWaveStrength");

      m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

         WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(m_scene->SceneProxies[PrimitiveProxyComponentId].get());
         proxyPtr->SetWaveStrength(m_waveStrength);
      });
   }

   void WaterPlaneComponent::SetTransparencyDepth(float transparencyDepth)
   {
      m_transparencyDepth = transparencyDepth;

      constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetTransparencyDepth");

      m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(),functionId, [=]() {

         WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(m_scene->SceneProxies[PrimitiveProxyComponentId].get());
         proxyPtr->SetTransparencyDepth(m_transparencyDepth);
      });
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

      constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetNearClipPlane");

      m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, functionId, GetObjectId(), [=]() {

         WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(m_scene->SceneProxies[PrimitiveProxyComponentId].get());
         proxyPtr->SetNearClipPlane(m_nearClipPlane);
      });
   }

   void WaterPlaneComponent::SetFarClipPlane(float farClipPlane)
   {
      m_farClipPlane = farClipPlane;

      constexpr uint64_t functionId = Hash("WaterPlaneComponent: SetFarClipPlane");

      m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, functionId, GetObjectId(), [=]() {

         WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(m_scene->SceneProxies[PrimitiveProxyComponentId].get());
         proxyPtr->SetFarClipPlane(m_farClipPlane);
      });
   }

}

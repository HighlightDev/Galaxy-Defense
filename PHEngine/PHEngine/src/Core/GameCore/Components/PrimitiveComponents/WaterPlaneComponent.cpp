#include "WaterPlaneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneProxy/WaterPlaneSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/WaterPlaneComponentData.h"

#include <glm/vec3.hpp>

using namespace Graphics::Renderer;

namespace EngineCore
{

   WaterPlaneComponent::WaterPlaneComponent(const WaterPlaneComponentData &data, const WaterPlaneRenderData &renderData,
                                            WaterQualityFlag waterQuality)
       : PrimitiveComponent(data.EngineObjectName, data.m_translation, data.m_eulerRotationDegrees, data.m_scale),
         m_waveSpeed(0.4f),
         m_renderData(renderData),
         m_waterQuality(waterQuality)
   {
   }

   WaterPlaneComponent::~WaterPlaneComponent()
   {
   }

   eComponentType WaterPlaneComponent::GetComponentType() const
   {
      return PRIMITIVE_COMPONENT;
   }

   void WaterPlaneComponent::UnpausableTick(const float deltaTime)
   {
      PrimitiveComponent::UnpausableTick(deltaTime);

      if (bIsRenderDataDirty)
      {
         SyncRenderData();
      }
   }

   float WaterPlaneComponent::GetWaveStrength() const
   {
      return m_waveStrength;
   }

   float WaterPlaneComponent::GetTransparencyDepth() const
   {
      return m_transparencyDepth;
   }

   void WaterPlaneComponent::SetWaveStrength(float waveStr)
   {
      if (!EngineMath::FloatsNearEqual(m_waveStrength, waveStr))
      {
         m_waveStrength = waveStr;
      }
   }

   void WaterPlaneComponent::SetTransparencyDepth(float transparencyDepth)
   {
      if (!EngineMath::FloatsNearEqual(m_transparencyDepth, transparencyDepth))
      {
         m_transparencyDepth = transparencyDepth;
         bIsRenderDataDirty = true;
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
      if (!EngineMath::FloatsNearEqual(nearClipPlane, m_nearClipPlane))
      {
         m_nearClipPlane = nearClipPlane;
         bIsRenderDataDirty = true;
      }
   }

   void WaterPlaneComponent::SetFarClipPlane(float farClipPlane)
   {
      if (!EngineMath::FloatsNearEqual(farClipPlane, m_farClipPlane))
      {
         m_farClipPlane = farClipPlane;
         bIsRenderDataDirty = true;
      }
   }

   void WaterPlaneComponent::SyncRenderData()
   {
      if (bIsRenderDataDirty)
      {
         if (const auto &sceneSp = m_sceneWP.lock())
         {
            if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
            {
               if (const auto &primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(SceneProxyId))
               {
                  bIsRenderDataDirty = false;
                  static const uint64_t functionId = Hash("WaterPlaneComponent::SyncRenderData");

                  sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, functionId, GetObjectId(), [=]() {
                     WaterPlaneSceneProxy* proxyPtr = static_cast<WaterPlaneSceneProxy*>(primitiveProxySp.get());
                     proxyPtr->SetFarClipPlane(m_farClipPlane); 
                     proxyPtr->SetNearClipPlane(m_nearClipPlane);
                     proxyPtr->SetTransparencyDepth(m_transparencyDepth);
                     proxyPtr->SetWaveStrength(m_waveStrength); 
                  });
               }
            }
         }
      }
   }
}

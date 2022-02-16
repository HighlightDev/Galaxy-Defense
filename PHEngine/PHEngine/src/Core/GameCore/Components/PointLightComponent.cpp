#include "PointLightComponent.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"

using namespace Graphics;

namespace Game
{

   PointLightComponent::PointLightComponent(const LightComponentData& lightComponentData)
      : LightComponent(lightComponentData)
   {
      const auto &d_pointLight = static_cast<const PointLightComponentData &>(lightComponentData);
      assert(nullptr == mLightRenderData);
      mLightRenderData = std::make_shared<PointLightRenderData>(d_pointLight.Attenuation, d_pointLight.RadianceRadius,
                                                            d_pointLight.Ambient, d_pointLight.Diffuse,
                                                            d_pointLight.Specular, d_pointLight.ShadowInfo);

      if (mLightRenderData->ShadowInfo)
      {
         PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);
         KinematicBodyMovedEvent::GetInstance()->AddListener(this);
         PlayerMovedEvent::GetInstance()->AddListener(this);
      }
   }

   PointLightComponent::~PointLightComponent()
   {
      if (mLightRenderData->ShadowInfo)
      {
         PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
         KinematicBodyMovedEvent::GetInstance()->RemoveListener(this);
         PlayerMovedEvent::GetInstance()->RemoveListener(this);
      }
   }

   std::shared_ptr<PointLightRenderData> PointLightComponent::GetRenderData() const
   {
      return std::static_pointer_cast<PointLightRenderData>(mLightRenderData);
   }

   std::shared_ptr<LightSceneProxy> PointLightComponent::CreateSceneProxy() const
   {
      return std::make_shared<PointLightSceneProxy>(this);
   }

   ComponentType PointLightComponent::GetComponentType() const
   {
      return LIGHT_COMPONENT;
   }

   void PointLightComponent::Tick(const float deltaTime)
   {
      Base::Tick(deltaTime);
   }

   void PointLightComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      auto &actorData = GetSerializeDataActor(dataContainer);

      auto lightCompData = std::make_shared<SerializeDataPointLightComponent>();
      const auto &pointLightRenderData = GetRenderData();

      lightCompData->ComponentName = GameObjectName;
      lightCompData->AmbientLight = pointLightRenderData->Ambient;
      lightCompData->DiffuseLight = pointLightRenderData->Diffuse;
      lightCompData->SpecularLight = pointLightRenderData->Specular;
      lightCompData->Translation = GetTranslation();
      lightCompData->Attenuation = pointLightRenderData->Attenuation;
      lightCompData->RadianceRadius = pointLightRenderData->RadianceRadius;

      const bool bHasShadowMap = pointLightRenderData->ShadowInfo != nullptr;

      if (bHasShadowMap)
      {
         lightCompData->ShadowMapSize = static_cast<float>(pointLightRenderData->ShadowInfo->GetAtlasResource()->GetTextureRezolution().x);
      }
      else
      {
         lightCompData->ShadowMapSize = 0.0f;
      }

      lightCompData->bHasShadowMap = bHasShadowMap;

      actorData.ComponentsData.emplace_back(lightCompData);
   }

   void PointLightComponent::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t &data)
   {
      static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::ProcessEvent(const KinematicBodyMovedEvent::EventData_t &data)
   {
      static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::ProcessEvent(const PlayerMovedEvent::EventData_t &data)
   {
      static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::NotifySceneProxyThatShadowmapIsDirty(const uint64_t &functionId)
   {
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, GetObjectId(), functionId, [=]()
                                           {
               auto proxy = sceneRenderer->LightProxiesMap[LightSceneProxyId];
               ProjectedShadowInfo* shadowInfo = proxy->GetShadowInfo();
               if (shadowInfo)
               {
                  shadowInfo->SetIsShadowMapDirty(true);
               } });
         }
      }
   }
}
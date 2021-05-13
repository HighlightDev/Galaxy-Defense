#include "PointLightComponent.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Event/PhysicsSimulationUpdatedEvent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"

using namespace Graphics;

namespace Game
{

   PointLightComponent::PointLightComponent(const std::string& gameObjectName, glm::vec3 translation, const PointLightRenderData& renderData)
      : LightComponent(gameObjectName, translation, glm::vec3(0), glm::vec3(1))
      , m_renderData(renderData)
   {
      if (renderData.ShadowInfo)
      {
         PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);
         KinematicBodyMovedEvent::GetInstance()->AddListener(this);
         PlayerMovedEvent::GetInstance()->AddListener(this);
      }
   }

   PointLightComponent::~PointLightComponent()
   {
      if (m_renderData.ShadowInfo)
      {
         PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
         KinematicBodyMovedEvent::GetInstance()->RemoveListener(this);
         PlayerMovedEvent::GetInstance()->RemoveListener(this);
      }
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

   void PointLightComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& actorData = GetSerializeDataActor(dataContainer);

      auto lightCompData = std::make_shared<SerializeDataPointLightComponent>();

      lightCompData->ComponentName = GameObjectName;
      lightCompData->AmbientLight = m_renderData.Ambient;
      lightCompData->DiffuseLight = m_renderData.Diffuse;
      lightCompData->SpecularLight = m_renderData.Specular;
      lightCompData->Translation = GetTranslation();
      lightCompData->Attenuation = m_renderData.Attenuation;
      lightCompData->RadianceRadius = m_renderData.RadianceRadius;

      const bool bHasShadowMap = !!m_renderData.ShadowInfo;

      if (bHasShadowMap)
      {
         lightCompData->ShadowMapSize = static_cast<float>(m_renderData.ShadowInfo->GetAtlasResource()->GetTextureRezolution().x);
      }
      else
      {
         lightCompData->ShadowMapSize = 0.0f;
      }

      lightCompData->bHasShadowMap = bHasShadowMap;

      actorData.ComponentsData.emplace_back(lightCompData);
   }

   void PointLightComponent::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data)
   {
      constexpr uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::ProcessEvent(const KinematicBodyMovedEvent::EventData_t& data)
   {
      constexpr uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::ProcessEvent(const PlayerMovedEvent::EventData_t& data)
   {
      constexpr uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::NotifySceneProxyThatShadowmapIsDirty(const uint64_t& functionId)
   {
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, GetObjectId(), functionId, [=]()
            {
               auto proxy = sceneRenderer->LightProxiesMap[LightSceneProxyId];
               ProjectedShadowInfo* shadowInfo = proxy->GetShadowInfo();
               if (shadowInfo)
               {
                  shadowInfo->SetIsShadowMapDirty(true);
               }
            });
         }
      }
   }

}
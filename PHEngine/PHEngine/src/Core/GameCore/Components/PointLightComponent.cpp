#include "PointLightComponent.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"

using namespace Graphics;

namespace EngineCore
{

   PointLightComponent::PointLightComponent(const std::shared_ptr<LightComponentData> &lightComponentData)
       : LightComponent(lightComponentData)
   {
      const auto &d_pointLight = std::static_pointer_cast<PointLightComponentData>(lightComponentData);
      assert(nullptr == mLightRenderData);
      mLightRenderData = std::make_shared<PointLightRenderData>(d_pointLight->Attenuation, d_pointLight->RadianceRadius,
                                                                d_pointLight->Ambient, d_pointLight->Diffuse,
                                                                d_pointLight->Specular, d_pointLight->ShadowInfo);
   }

   PointLightComponent::~PointLightComponent()
   {
      if (mLightRenderData->ShadowInfo)
      {
         PhysicsComponentUpdatedGameThreadEvent::GetInstance()->RemoveListener(PhysicsComponentUpdatedGameThreadEvent::GetInstanceId());
         KinematicBodyMovedGameThreadEvent::GetInstance()->RemoveListener(KinematicBodyMovedGameThreadEvent::GetInstanceId());
         PlayerMovedGameThreadEvent::GetInstance()->RemoveListener(PlayerMovedGameThreadEvent::GetInstanceId());
      }
   }

   void PointLightComponent::Initialize()
   {
      if (mLightRenderData->ShadowInfo)
      {
         const auto thisSp = std::dynamic_pointer_cast<PointLightComponent>(shared_from_this());
         PhysicsComponentUpdatedGameThreadEvent::GetInstance()->AddListener(thisSp);
         KinematicBodyMovedGameThreadEvent::GetInstance()->AddListener(thisSp);
         PlayerMovedGameThreadEvent::GetInstance()->AddListener(thisSp);
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

   eComponentType PointLightComponent::GetComponentType() const
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

      lightCompData->ComponentName = EngineObjectName;
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

   void PointLightComponent::ProcessEvent(const PhysicsComponentUpdatedGameThreadEvent::EventData_t &data)
   {
      static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::ProcessEvent(const KinematicBodyMovedGameThreadEvent::EventData_t &data)
   {
      static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::ProcessEvent(const PlayerMovedGameThreadEvent::EventData_t &data)
   {
      static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      NotifySceneProxyThatShadowmapIsDirty(functionId);
   }

   void PointLightComponent::NotifySceneProxyThatShadowmapIsDirty(const uint64_t &functionId)
   {
      if (const auto &sceneSp = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
         {
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, GetObjectId(), functionId, [=]()
                                                                                {
               const auto& lightProxySp = sceneRenderer->GetLightProxyByProxyId(mLightSceneProxyId);
               const auto& shadowInfo = lightProxySp->GetShadowInfo();
               if (shadowInfo)
               {
                  shadowInfo->SetIsShadowMapDirty(true);
               } });
         }
      }
   }
}
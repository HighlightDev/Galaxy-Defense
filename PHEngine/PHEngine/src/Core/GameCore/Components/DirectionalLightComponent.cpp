#include "DirectionalLightComponent.h"

#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"

#include <glm/gtx/quaternion.hpp>

using namespace Graphics;

namespace EngineCore
{

   DirectionalLightComponent::DirectionalLightComponent(const LightComponentData &lightComponentData)
       : LightComponent(lightComponentData)
   {
      const auto &d_directionalLight = static_cast<const DirectionalLightComponentData &>(lightComponentData);
      assert(nullptr == mLightRenderData);
      mLightRenderData = std::make_shared<DirectionalLightRenderData>(d_directionalLight.Direction, d_directionalLight.Ambient,
                                                                      d_directionalLight.Diffuse, d_directionalLight.Specular,
                                                                      d_directionalLight.ShadowInfo);

      if (mLightRenderData->ShadowInfo)
      {
         PlayerMovedEvent::GetInstance()->AddListener(this);
         PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);
      }
   }

   DirectionalLightComponent::~DirectionalLightComponent()
   {
      if (mLightRenderData->ShadowInfo)
      {
         PlayerMovedEvent::GetInstance()->RemoveListener(this);
         PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
      }
   }

   std::shared_ptr<DirectionalLightRenderData> DirectionalLightComponent::GetRenderData() const
   {
      return std::static_pointer_cast<DirectionalLightRenderData>(mLightRenderData);
   }

   std::shared_ptr<LightSceneProxy> DirectionalLightComponent::CreateSceneProxy() const
   {
      return std::make_shared<DirectionalLightSceneProxy>(this);
   }

   void DirectionalLightComponent::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {

      auto &actorData = GetSerializeDataActor(dataContainer);

      auto lightCompData = std::make_shared<SerializeDataDirLightComponent>();
      const auto &renderData = GetRenderData();

      lightCompData->ComponentName = GameObjectName;
      lightCompData->AmbientLight = renderData->Ambient;
      lightCompData->DiffuseLight = renderData->Diffuse;
      lightCompData->SpecularLight = renderData->Specular;
      lightCompData->Direction = renderData->Direction;
      lightCompData->Rotation = GetRotationEuler();

      const bool bHasShadowMap = renderData->ShadowInfo != nullptr;

      if (bHasShadowMap)
      {
         lightCompData->ShadowMapSize = (float)renderData->ShadowInfo->GetAtlasResource()->GetTextureRezolution().x;
      }
      else
      {
         lightCompData->ShadowMapSize = 0.0f;
      }

      lightCompData->bHasShadowMap = bHasShadowMap;

      actorData.ComponentsData.emplace_back(lightCompData);
   }

   void DirectionalLightComponent::UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);
   }

   void DirectionalLightComponent::Tick(float deltaTime)
   {
      Base::Tick(deltaTime);
   }

   ComponentType DirectionalLightComponent::GetComponentType() const
   {
      return ComponentType::LIGHT_COMPONENT;
   }

   void DirectionalLightComponent::ForceUpdateShadowMap()
   {
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            static const uint64_t functionId = Hash("DirectionalLightComponent: ForceUpdateShadowMap");

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

   void DirectionalLightComponent::ProcessEvent(const PlayerMovedEvent::EventData_t &data)
   {
      if (const auto &sceneSP = m_sceneWP.lock())
      {
         if (const auto &sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            static const uint64_t functionId = Hash("DirectionalLightComponent: Set shadowInfo->Offset");

            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]()
                                           {
               auto proxy = sceneRenderer->LightProxiesMap[LightSceneProxyId];
               if (  ProjectedShadowInfo* shadowInfo = proxy->GetShadowInfo())
               {
                  if (const auto& transformSp = std::get<0>(data).lock())
                  {
                     shadowInfo->SetPlayerPositionOffset(transformSp->Translation);
                  }
                  proxy->SetIsTransformationDirty(true);
                  shadowInfo->SetIsShadowMapDirty(true);
               } });
         }
      }
   }

   void DirectionalLightComponent::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t &data)
   {
      ForceUpdateShadowMap();
   }

}

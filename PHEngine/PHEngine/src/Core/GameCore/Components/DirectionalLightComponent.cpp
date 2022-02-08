#include "DirectionalLightComponent.h"

#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtx/quaternion.hpp>

using namespace Graphics;

namespace Game
{

   DirectionalLightComponent::DirectionalLightComponent(const std::string& gameObjectName, glm::vec3 rotation, const DirectionalLightRenderData& renderData)
      : LightComponent(gameObjectName, glm::vec3(0), rotation, glm::vec3(1))
      , m_renderData(renderData)
   {
      if (renderData.ShadowInfo)
      {
         PlayerMovedEvent::GetInstance()->AddListener(this);
         PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);
      }
   }

   DirectionalLightComponent::~DirectionalLightComponent()
   {
      if (m_renderData.ShadowInfo)
      {
         PlayerMovedEvent::GetInstance()->RemoveListener(this);
         PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
      }
   }

   std::shared_ptr<LightSceneProxy> DirectionalLightComponent::CreateSceneProxy() const
   {
      return std::make_shared<DirectionalLightSceneProxy>(this);
   }

   void DirectionalLightComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer) {

      auto& actorData = GetSerializeDataActor(dataContainer);

      auto lightCompData = std::make_shared<SerializeDataDirLightComponent>();

      lightCompData->ComponentName = GameObjectName;
      lightCompData->AmbientLight = m_renderData.Ambient;
      lightCompData->DiffuseLight = m_renderData.Diffuse;
      lightCompData->SpecularLight = m_renderData.Specular;
      lightCompData->Direction = m_renderData.Direction;
      lightCompData->Rotation = GetRotationEuler();

      const bool bHasShadowMap = !!m_renderData.ShadowInfo;

      if (bHasShadowMap)
      {
         lightCompData->ShadowMapSize = (float)m_renderData.ShadowInfo->GetAtlasResource()->GetTextureRezolution().x;
      }
      else 
      {
         lightCompData->ShadowMapSize = 0.0f;
      }

      lightCompData->bHasShadowMap = bHasShadowMap;

      actorData.ComponentsData.emplace_back(lightCompData);
   }

   void DirectionalLightComponent::UpdateRelativeMatrix(const glm::mat4& parentRelativeMatrix)
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
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            static const uint64_t functionId = Hash("DirectionalLightComponent: ForceUpdateShadowMap");

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

   void DirectionalLightComponent::ProcessEvent(const PlayerMovedEvent::EventData_t& data)
   {
      if (const auto& sceneSP = m_sceneWP.lock())
      {
         if (const auto& sceneRenderer = sceneSP->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            static const uint64_t functionId = Hash("DirectionalLightComponent: Set shadowInfo->Offset");

            sceneSP->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]()
            {
               auto proxy = sceneRenderer->LightProxiesMap[LightSceneProxyId];
               ProjectedShadowInfo* shadowInfo = proxy->GetShadowInfo();
               if (shadowInfo)
               {
                  std::weak_ptr<Transform> playerTransformWP = std::get<0>(data);
                  if (auto transform = playerTransformWP.lock())
                  {
                     shadowInfo->SetPlayerPositionOffset(transform->Translation);
                  }
                  proxy->SetIsTransformationDirty(true);
                  shadowInfo->SetIsShadowMapDirty(true);
               }
            });
         }
      }
   }

   void DirectionalLightComponent::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data)
   {
      ForceUpdateShadowMap();
   }

}

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

   DirectionalLightComponent::DirectionalLightComponent(glm::vec3 rotation, const DirectionalLightRenderData& renderData)
      : LightComponent(glm::vec3(0), rotation, glm::vec3(1))
      , m_renderData(renderData)
   {
      PlayerMovedEvent::GetInstance()->AddListener(this);
      PhysicsSimulationUpdatedEvent::GetInstance()->AddListener(this);
   }

   DirectionalLightComponent::~DirectionalLightComponent()
   {
      PlayerMovedEvent::GetInstance()->RemoveListener(this);
      PhysicsSimulationUpdatedEvent::GetInstance()->RemoveListener(this);
   }

   std::shared_ptr<LightSceneProxy> DirectionalLightComponent::CreateSceneProxy() const
   {
      return std::make_shared<DirectionalLightSceneProxy>(this);
   }

   void DirectionalLightComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
   {
      Base::UpdateRelativeMatrix(parentRelativeMatrix);
   }

   void DirectionalLightComponent::Tick(float deltaTime)
   {
      Base::Tick(deltaTime);

      SetRotator(mTransform->Rotator * glm::angleAxis(DEG_TO_RAD(deltaTime * 8), AXIS_UP));
      ProcessEvent(PhysicsSimulationUpdatedEvent::EventData_t());
   }

   uint64_t DirectionalLightComponent::GetComponentType() const
   {
      return DIR_LIGHT_COMPONENT;
   }

   void DirectionalLightComponent::ProcessEvent(const PlayerMovedEvent::EventData_t& data)
   {
      if (const auto& sceneRenderer = m_scene->GetThreadManager().TryGetSceneRendererWP().lock())
      {
         constexpr uint64_t functionId = Hash("DirectionalLightComponent: Set shadowInfo->Offset");

         m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]()
         {
            auto proxy = sceneRenderer->LightProxies[LightSceneProxyId];
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

   void DirectionalLightComponent::ProcessEvent(const PhysicsSimulationUpdatedEvent::EventData_t& data)
   {
      if (const auto& sceneRenderer = m_scene->GetThreadManager().TryGetSceneRendererWP().lock())
      {
         constexpr uint64_t functionId = Hash("DirectionalLightComponent: Set shadowInfo->bMustUpdateShadowmap");

         m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, GetObjectId(), functionId, [=]()
         {
            auto proxy = sceneRenderer->LightProxies[LightSceneProxyId];
            ProjectedShadowInfo* shadowInfo = proxy->GetShadowInfo();
            if (shadowInfo)
            {
               shadowInfo->SetIsShadowMapDirty(true);
            }
         });
      }
   }

}

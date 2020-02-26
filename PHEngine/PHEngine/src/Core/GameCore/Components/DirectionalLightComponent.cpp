#include "DirectionalLightComponent.h"

#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "core/CommonApi/StringHash.h"

using namespace Graphics;

namespace Game
{

   DirectionalLightComponent::DirectionalLightComponent(glm::vec3 rotation, const DirectionalLightRenderData& renderData)
      : LightComponent(glm::vec3(0), rotation, glm::vec3(1))
      , m_renderData(renderData)
   {
      PlayerMovedEvent::GetInstance()->AddListener(this);
      SceneComponentTransformChangedEvent::GetInstance()->AddListener(this);
   }

   DirectionalLightComponent::~DirectionalLightComponent()
   {
      PlayerMovedEvent::GetInstance()->RemoveListener(this);
      SceneComponentTransformChangedEvent::GetInstance()->RemoveListener(this);
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

     /* SetRotationAxisX(m_rotation.x + 0.001f);
      SetRotationAxisY(m_rotation.y + 0.001f);
      SetRotationAxisZ(m_rotation.z + 0.001f);*/
   }

   uint64_t DirectionalLightComponent::GetComponentType() const
   {
      return DIR_LIGHT_COMPONENT;
   }

   void DirectionalLightComponent::ProcessEvent(const PlayerMovedEvent::EventData_t& data)
   {
      constexpr uint64_t functionId = Hash("DirectionalLightComponent: Set shadowInfo->Offset");

      m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]()
      {
         auto proxy = m_scene->LightProxies[LightSceneProxyId];
         ProjectedShadowInfo* shadowInfo = proxy->GetShadowInfo();
         if (shadowInfo)
         {
            shadowInfo->Offset = std::get<0>(data);
            proxy->SetIsTransformationDirty(true);
         }
      });
   }

   void DirectionalLightComponent::ProcessEvent(const SceneComponentTransformChangedEvent::EventData_t& data)
   {
      constexpr uint64_t functionId = Hash("DirectionalLightComponent: Set shadowInfo->bMustUpdateShadowmap");

      m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_NO_PUSH, GetObjectId(), functionId, [=]()
      {
         auto proxy = m_scene->LightProxies[LightSceneProxyId];
         ProjectedShadowInfo* shadowInfo = proxy->GetShadowInfo();
         if (shadowInfo)
         {
            shadowInfo->bMustUpdateShadowmap = true;
         }
      });
   }

}

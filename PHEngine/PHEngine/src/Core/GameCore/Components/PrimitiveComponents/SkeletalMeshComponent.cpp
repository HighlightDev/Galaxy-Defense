#include "SkeletalMeshComponent.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"
#include "Core/GameCore/Scene.h"
#include "Core/CommonApi/StringHash.h"
#include "Core/GameCore/Event/SceneComponentTransformChangedEvent.h"

using namespace Graphics::Proxy;

namespace Game
{

   SkeletalMeshComponent::SkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const SkeletalMeshRenderData& renderData)
      : PrimitiveComponent(translation, rotation, scale)
      , m_renderData(renderData)
      , m_animationDeltaTime(.0f)
   {

   }

   SkeletalMeshComponent::~SkeletalMeshComponent()
   {
   }

   uint64_t SkeletalMeshComponent::GetComponentType() const
   {
      return SKELETAL_MESH_COMPONENT;
   }

   void SkeletalMeshComponent::Tick(float deltaTime)
   {
      m_animationDeltaTime = std::max(deltaTime, 0.005f);

      constexpr uint64_t functionId = Hash("SkeletalMeshComponent: SetAnimationDeltaTime");

      m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

         SkeletalMeshSceneProxy* proxyPtr = static_cast<SkeletalMeshSceneProxy*>(m_scene->SceneProxies[PrimitiveProxyComponentId].get());
         proxyPtr->SetAnimationDeltaTime(m_animationDeltaTime);
      });

      Event::SceneComponentTransformChangedEvent::GetInstance()->SendEvent(GetComponentType());
   }

   std::shared_ptr<PrimitiveSceneProxy> SkeletalMeshComponent::CreateSceneProxy() const
   {
      return std::make_shared<SkeletalMeshSceneProxy>(this);
   }

   float SkeletalMeshComponent::GetAnimationDeltaTime() const
   {
      return m_animationDeltaTime;
   }

}
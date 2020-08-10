#include "PlayerSkeletalMeshComponent.h"

#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"

#include "Core/GameCore/Scene.h"

namespace Labyrinth
{
   PlayerSkeletalMeshComponent::PlayerSkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale,
      const SkeletalMeshRenderData& renderData)
      : SkeletalMeshComponent(translation, rotation, scale, renderData)
   {
   }

   PlayerSkeletalMeshComponent::~PlayerSkeletalMeshComponent()
   {
   }

   void PlayerSkeletalMeshComponent::Tick(float deltaTime)
   {
      const float animDeltaTime = deltaTime * 10;

      constexpr uint64_t functionId = Hash("SkeletalMeshComponent: SetAnimationDeltaTime");

      mSrcAnimationTime += animDeltaTime;
      mSrcAnimationTime = fmod(mSrcAnimationTime, 1000000.0f);

      if (const auto& sceneRenderer = m_scene->GetThreadManager().TryGetSceneRendererWP().lock())
      {
         m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

            SkeletalMeshSceneProxy* proxyPtr = static_cast<SkeletalMeshSceneProxy*>(sceneRenderer->SceneProxies[PrimitiveProxyComponentId].get());
            proxyPtr->UpdateAnimationData(bTransitionEnabled, mTransitionValue, mSrcAnimationTime, mDstAnimationTime, mSrcAnimationName, mDstAnimationName);
         });
      }
   }
  
}
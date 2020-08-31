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
      const float animDeltaTime = deltaTime * 90.0f;
      mSrcAnimationTime += animDeltaTime;

      mUpdateDataResetTimeCounter += deltaTime;
      const bool bUpdateData = mUpdateDataResetTimeCounter >= update_data_reset_time;
      mUpdateDataResetTimeCounter = fmod(mUpdateDataResetTimeCounter, update_data_reset_time);

      if (bUpdateData)
      {
         if (const auto& sceneRenderer = m_scene->GetThreadManager().TryGetSceneRendererWP().lock())
         {
            static constexpr uint64_t functionId = Hash("SkeletalMeshComponent: SetAnimationDeltaTime");
            mSrcAnimationTime = fmod(mSrcAnimationTime, 1000000.0f);

            m_scene->ExecuteOnRenderThread(EnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH, GetObjectId(), functionId, [=]() {

               SkeletalMeshSceneProxy* proxyPtr = static_cast<SkeletalMeshSceneProxy*>(sceneRenderer->SceneProxies[PrimitiveProxyComponentId].get());
               proxyPtr->UpdateAnimationData(bTransitionEnabled, mTransitionValue, mSrcAnimationTime, mDstAnimationTime, mSrcAnimationName, mDstAnimationName);
            });
         }
      }
   }
  
}
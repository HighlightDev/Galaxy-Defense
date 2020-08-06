#include "PlayerSkeletalMeshComponent.h"

#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/SkeletalMeshSceneProxy.h"

#include "Core/GameCore/Scene.h"

namespace Labyrinth
{
   PlayerSkeletalMeshComponent::PlayerSkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale,
      const SkeletalMeshRenderData& renderData)
      : SkeletalMeshComponent(translation, rotation, scale, renderData)
      , mAnimationStateMachine(nullptr)
      , mPropertiesBinding(nullptr)
   {
      InitStateMachine();
   }

   PlayerSkeletalMeshComponent::~PlayerSkeletalMeshComponent()
   {
      delete mAnimationStateMachine;
   }

   int tickerdebug = 0;

   void PlayerSkeletalMeshComponent::Tick(float deltaTime)
   {
      const float animDeltaTime = deltaTime;

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

      mAnimationStateMachine->Tick(deltaTime);
      tickerdebug++;
      if (tickerdebug == 100)
      {
         mAnimationStateMachine->ChangeState("State Walking");
      }
   }

   void PlayerSkeletalMeshComponent::ChangeState()
   {

   }

   void PlayerSkeletalMeshComponent::InitStateMachine()
   {
      // Init reference binding
      mPropertiesBinding = std::make_shared<AnimationPropertyBinding>(mSrcAnimationName, mDstAnimationName,
         mSrcAnimationTime, mDstAnimationTime, bTransitionEnabled, mTransitionValue);

      State* stateIdle = new State("State Idle");
      State* stateWalking = new State("State Walking");

      StateProperty<StatePropertyType::Animation>* prop_idleAnim = new StateProperty<StatePropertyType::Animation>("Iddle", mPropertiesBinding);
      stateIdle->AddStateProperty("Prop_AnimationTina", prop_idleAnim);

      StateTransition transitionFromIdleToWalking(stateIdle, stateWalking, 1.0f);
      stateIdle->AddStateTransition(transitionFromIdleToWalking);

      StateTransition transitionFromWalkingToIdle(stateWalking, stateIdle, 1.0f);
      stateWalking->AddStateTransition(transitionFromWalkingToIdle);

      StateProperty<StatePropertyType::Animation>* prop_walkAnim = new StateProperty<StatePropertyType::Animation>("Armature|Walk", mPropertiesBinding);
      stateWalking->AddStateProperty("Prop_AnimationTina", prop_walkAnim);

      mAnimationStateMachine = new StateMachine(stateIdle);
   }

}
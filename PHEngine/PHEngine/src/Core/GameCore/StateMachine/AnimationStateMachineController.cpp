#include "AnimationStateMachineController.h"
#include "StateProperty.h"

namespace Game
{

   AnimationStateMachineController::AnimationStateMachineController(std::weak_ptr<AnimationPlayer> animationPlayer)
      : mAnimationPlayer(animationPlayer)
   {
   }

   void AnimationStateMachineController::MakeTransitionToState(BaseStateProperty* srcProperty,
      BaseStateProperty* dstProperty,
      const float transitionDuration)
   {
      Base::MakeTransitionToState(srcProperty, dstProperty, transitionDuration);

      StateProperty<StatePropertyType::Animation>* srcAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(srcProperty);
      StateProperty<StatePropertyType::Animation>* dstAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(dstProperty);

      if (std::shared_ptr<AnimationPlayer> spAnimPlayer = mAnimationPlayer.lock())
      {
         spAnimPlayer->mDstAnimationName = dstAnimationProperty->AnimationName; // dstState;
      }
   }

   void AnimationStateMachineController::UpdateTransitionTime(const float deltaTime)
   {
      Base::UpdateTransitionTime(deltaTime);

      if (bTransitionEnabled)
      {
         if (std::shared_ptr<AnimationPlayer> spAnimPlayer = mAnimationPlayer.lock())
         {
            spAnimPlayer->mDstAnimationTime += deltaTime;
         }
      }
   }

   void AnimationStateMachineController::OnTransitionFinished()
   {
      if (std::shared_ptr<AnimationPlayer> spAnimPlayer = mAnimationPlayer.lock())
      {
        spAnimPlayer->mSrcAnimationName = spAnimPlayer->mDstAnimationName;
        spAnimPlayer->mSrcAnimationTime = spAnimPlayer->mDstAnimationTime;
        spAnimPlayer->mDstAnimationTime = 0.0f;
        spAnimPlayer->mDstAnimationName = "NoAnimation";
      }
   }

}
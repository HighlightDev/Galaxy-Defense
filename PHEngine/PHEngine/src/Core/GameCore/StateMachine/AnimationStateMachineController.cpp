#include "AnimationStateMachineController.h"
#include "StateProperty.h"

namespace Game
{
   AnimationStateMachineController::AnimationStateMachineController()
      : mSrcAnimationTime(0)
      , mDstAnimationTime(0)
      , mSrcAnimationName("NoAnimation")
      , mDstAnimationName("NoAnimation")
   {
   }

   void AnimationStateMachineController::OnTransitionUpdate(const float deltaTime)
   {
      Base::OnTransitionUpdate(deltaTime);

      mDstAnimationTime += deltaTime;
      mSrcAnimationTime += deltaTime;
   }

   void AnimationStateMachineController::OnTransitionStarted(BaseStateProperty* srcProperty,
      BaseStateProperty* dstProperty,
      const float transitionDuration)
   {
      Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

      StateProperty<StatePropertyType::Animation>* srcAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(srcProperty);
      StateProperty<StatePropertyType::Animation>* dstAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(dstProperty);

      mSrcAnimationName = srcAnimationProperty->AnimationName;
      mDstAnimationName = dstAnimationProperty->AnimationName;
      mDstAnimationTime = 0.0f;
   }

   void AnimationStateMachineController::OnTransitionFinished()
   {
      mSrcAnimationName = mDstAnimationName;
      mSrcAnimationTime = mDstAnimationTime;
      mDstAnimationTime = 0.0f;
      mDstAnimationName = "NoAnimation";
   }

   float AnimationStateMachineController::GetSrcAnimationTime() const
   {
      return mSrcAnimationTime;
   }

   float AnimationStateMachineController::GetDstAnimationTime() const
   {
      return mDstAnimationTime;
   }

   std::string AnimationStateMachineController::GetSrcAnimationName() const
   {
      return mSrcAnimationName;
   }

   std::string AnimationStateMachineController::GetDstAnimationName() const
   {
      return mDstAnimationName;
   }
}
#include "AnimationStateMachineController.h"
#include "StateProperty.h"

namespace Game
{
   AnimationStateMachineController::AnimationStateMachineController()
      : IStateMachineController()
   {
   }

   std::shared_ptr<AnimationPropertyBinding> AnimationStateMachineController::GetAnimationPropertyBindingSP() const
   {
      std::shared_ptr<AnimationPropertyBinding> result(nullptr);

      if (auto baseSp = mPropertyBinding.lock()) {
         result = std::static_pointer_cast<AnimationPropertyBinding>(baseSp);
      }

      return result;
   }

   void AnimationStateMachineController::OnTransitionUpdate(const float deltaTime, const float transitionParameter)
   {
      Base::OnTransitionUpdate(deltaTime, transitionParameter);

      if (auto animBinding = GetAnimationPropertyBindingSP())
      {
         animBinding->DstTime += deltaTime;
         animBinding->SrcTime += deltaTime;
         animBinding->bTranstitionEnabled = true;
         animBinding->TransitionValue = transitionParameter;
      }
   }

   void AnimationStateMachineController::OnTransitionStarted(
      BaseStateProperty* srcProperty,
      BaseStateProperty* dstProperty,
      const float transitionDuration)
   {
      Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

      StateProperty<StatePropertyType::Animation>* srcAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(srcProperty);
      StateProperty<StatePropertyType::Animation>* dstAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(dstProperty);

      if (auto animBinding = GetAnimationPropertyBindingSP())
      {
         animBinding->SrcName = srcAnimationProperty->AnimationName;
         animBinding->DstName = dstAnimationProperty->AnimationName;
         animBinding->DstTime = 0.0f;
         animBinding->bTranstitionEnabled = true;
         animBinding->TransitionValue = 0.0f;
      }
   }

   void AnimationStateMachineController::OnTransitionFinished()
   {
      if (auto animBinding = GetAnimationPropertyBindingSP())
      {
         animBinding->SrcName = animBinding->DstName;
         animBinding->SrcTime = animBinding->DstTime;
         animBinding->DstTime = 0.0f;
         animBinding->DstName = "NoAnimation";
         animBinding->bTranstitionEnabled = false;
         animBinding->TransitionValue = 0.0f;
      }
   }
}
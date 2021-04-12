#include "AnimationTweenController.h"
#include "StateProperty.h"

namespace Game
{
   AnimationTweenController::AnimationTweenController()
      : ITweenController()
   {
   }

   std::shared_ptr<AnimationPropertyBinding> AnimationTweenController::GetAnimationPropertyBindingSP() const
   {
      std::shared_ptr<AnimationPropertyBinding> result(nullptr);

      if (auto baseSp = mPropertyBinding.lock()) {
         result = std::static_pointer_cast<AnimationPropertyBinding>(baseSp);
      }

      return result;
   }

   void AnimationTweenController::OnTransitionUpdate(const float deltaTime, const float transitionParameter)
   {
      Base::OnTransitionUpdate(deltaTime, transitionParameter);

      if (auto animBinding = GetAnimationPropertyBindingSP())
      {
         *animBinding->DstTime += deltaTime;
         *animBinding->bTranstitionEnabled = true;
         *animBinding->TransitionValue = transitionParameter;
      }
   }

   void AnimationTweenController::InitWithPropsInstant(struct BaseStateProperty* dstStateProperty)
   {
      mPropertyBinding = dstStateProperty->PropertyBinding;
      if (auto animBinding = GetAnimationPropertyBindingSP())
      {
         StateProperty<StatePropertyType::Animation>* dstAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(dstStateProperty);

         *animBinding->SrcName = dstAnimationProperty->AnimationName;
         *animBinding->SrcTime = 0.0f;
         *animBinding->DstTime = 0.0f;
         *animBinding->DstName = "NoAnimation";
         *animBinding->bTranstitionEnabled = false;
         *animBinding->TransitionValue = 0.0f;
      }
   }

   void AnimationTweenController::OnTransitionStarted(
      BaseStateProperty* srcProperty,
      BaseStateProperty* dstProperty,
      const float transitionDuration)
   {
      Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

      StateProperty<StatePropertyType::Animation>* srcAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(srcProperty);
      StateProperty<StatePropertyType::Animation>* dstAnimationProperty = static_cast<StateProperty<StatePropertyType::Animation>*>(dstProperty);

      if (auto animBinding = GetAnimationPropertyBindingSP())
      {
         assert(animBinding->SrcName != nullptr);

         *animBinding->SrcName = srcAnimationProperty->AnimationName;
         *animBinding->DstName = dstAnimationProperty->AnimationName;
         *animBinding->DstTime = 0.0f;
         *animBinding->bTranstitionEnabled = true;
         *animBinding->TransitionValue = 0.0f;
      }
   }

   void AnimationTweenController::OnTransitionFinished()
   {
      if (auto animBinding = GetAnimationPropertyBindingSP())
      {
         *animBinding->SrcName = *animBinding->DstName;
         *animBinding->SrcTime = *animBinding->DstTime;
         *animBinding->DstTime = 0.0f;
         *animBinding->DstName = "NoAnimation";
         *animBinding->bTranstitionEnabled = false;
         *animBinding->TransitionValue = 0.0f;
      }
   }
}
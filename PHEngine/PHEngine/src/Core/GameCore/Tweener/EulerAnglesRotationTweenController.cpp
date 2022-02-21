#include "EulerAnglesRotationTweenController.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/ext/quaternion_float.hpp>

namespace Game
{

   EulerAnglesRotationTweenController::EulerAnglesRotationTweenController()
       : ITweenController()
   {
   }

   EulerAnglesRotationTweenController::~EulerAnglesRotationTweenController()
   {
   }

   std::shared_ptr<EulerAnglesRotationPropertyBinding> EulerAnglesRotationTweenController::GetRotationPropertyBindingSP() const
   {
      std::shared_ptr<EulerAnglesRotationPropertyBinding> result(nullptr);

      if (auto baseSp = mPropertyBinding.lock())
      {
         result = std::static_pointer_cast<EulerAnglesRotationPropertyBinding>(baseSp);
         assert(result);
      }

      return result;
   }

   void EulerAnglesRotationTweenController::OnTransitionUpdate(const float deltaTime, const float transitionParameter)
   {
      Base::OnTransitionUpdate(deltaTime, transitionParameter);

      if (auto rotationBinding = GetRotationPropertyBindingSP())
      {
         auto srcRotationProperty =
             static_cast<TweenStateProperty_t *>(TranstionProperties[(int)StateType::SourceState]);
         auto dstRotationProperty =
             static_cast<TweenStateProperty_t *>(TranstionProperties[(int)StateType::DestinationState]);

         glm::quat srcQuat = srcRotationProperty->QuatValue;
         glm::quat dstQuat = dstRotationProperty->QuatValue;

         rotationBinding->SetValue(EngineMath::QuatToEulerAngles(EngineMath::SLerpQuat(transitionParameter, srcQuat, dstQuat)));
      }
   }

   void EulerAnglesRotationTweenController::InitWithPropsInstant(struct BaseStateProperty *dstStateProperty)
   {
      mPropertyBinding = dstStateProperty->Binding;
      if (auto rotationBinding = GetRotationPropertyBindingSP())
      {
         auto dstFloatProperty = static_cast<TweenStateProperty_t *>(dstStateProperty);

         rotationBinding->SetValue(dstFloatProperty->Value);
      }
   }

   void EulerAnglesRotationTweenController::OnTransitionStarted(
       BaseStateProperty *srcProperty,
       BaseStateProperty *dstProperty,
       const float transitionDuration)
   {
      Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

      if (auto rotationBinding = GetRotationPropertyBindingSP())
      {
         auto srcFloatProperty = static_cast<TweenStateProperty_t *>(TranstionProperties[(int)StateType::SourceState]);

         rotationBinding->SetValue(srcFloatProperty->Value);
      }
   }

   void EulerAnglesRotationTweenController::OnTransitionFinished()
   {
      if (auto rotationBinding = GetRotationPropertyBindingSP())
      {
         auto dstFloatProperty = static_cast<TweenStateProperty_t *>(TranstionProperties[(int)StateType::DestinationState]);

         rotationBinding->SetValue(dstFloatProperty->Value);
      }
   }

}

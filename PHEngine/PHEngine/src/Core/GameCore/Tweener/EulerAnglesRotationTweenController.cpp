#include "EulerAnglesRotationTweenController.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/ext/quaternion_float.hpp>

namespace EngineCore
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
             std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
         auto dstRotationProperty =
             std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

         glm::quat srcQuat = srcRotationProperty->QuatValue;
         glm::quat dstQuat = dstRotationProperty->QuatValue;

         rotationBinding->SetValue(EngineMath::QuatToEulerAngles(EngineMath::SLerpQuat(transitionParameter, srcQuat, dstQuat)));
      }
   }

   void EulerAnglesRotationTweenController::InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty)
   {
      mPropertyBinding = dstStateProperty->Binding;
      if (auto rotationBinding = GetRotationPropertyBindingSP())
      {
         auto dstRotationProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

         rotationBinding->SetValue(dstRotationProperty->Value);
      }
   }

   void EulerAnglesRotationTweenController::OnTransitionStarted(
       const std::shared_ptr<BaseStateProperty>& srcProperty,
       const std::shared_ptr<BaseStateProperty>& dstProperty,
       const float transitionDuration)
   {
      Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

      if (auto rotationBinding = GetRotationPropertyBindingSP())
      {
         auto srcRotationProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

         rotationBinding->SetValue(srcRotationProperty->Value);
      }
   }

   void EulerAnglesRotationTweenController::OnTransitionFinished()
   {
      if (auto rotationBinding = GetRotationPropertyBindingSP())
      {
         auto dstRotationProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

         rotationBinding->SetValue(dstRotationProperty->Value);
      }
   }

}

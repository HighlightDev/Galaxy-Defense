#include "ITweenController.h"
#include "StateProperty.h"
#include "Core/CommonCore/Assertion.h"

#include <iostream>

namespace Game {

   ITweenController::ITweenController()
   {
   }

   void ITweenController::OnTransitionStarted(BaseStateProperty* srcStateProperty, BaseStateProperty* dstStateProperty, const float transitionDuration)
   {
      mPropertyBinding = srcStateProperty->PropertyBinding;
      TranstionProperties[(int)StateType::SourceState] = srcStateProperty;
      TranstionProperties[(int)StateType::DestinationState] = dstStateProperty;
   }

   void ITweenController::OnTransitionUpdate(const float deltaTime, const float transitionParameter) { }

   StatePropertyType ITweenController::GetControllerPropertyType() const
   {
      BaseStateProperty* srcProp = TranstionProperties[(int)StateType::SourceState];

      assert(srcProp != nullptr);

      return srcProp->GetStatePropertyType();
   }

   void ITweenController::InitWithPropsInstant(struct BaseStateProperty* dstStateProperty)
   {

   }
}
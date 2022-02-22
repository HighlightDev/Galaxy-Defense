#include "ITweenController.h"
#include "StateProperty.h"
#include "Core/CommonCore/Assertion.h"

#include <iostream>

namespace EngineCore {

   ITweenController::ITweenController()
   {
   }

   void ITweenController::OnTransitionStarted(BaseStateProperty* srcStateProperty, BaseStateProperty* dstStateProperty, const float transitionDuration)
   {
      mPropertyBinding = srcStateProperty->Binding;
      TranstionProperties[(int)StateType::SourceState] = srcStateProperty;
      TranstionProperties[(int)StateType::DestinationState] = dstStateProperty;
   }

   void ITweenController::OnTransitionUpdate(const float deltaTime, const float transitionParameter) { }

   eBindingType ITweenController::GetControllerPropertyType() const
   {
      BaseStateProperty* srcProp = TranstionProperties[(int)StateType::SourceState];

      assert(srcProp != nullptr);

      return srcProp->GetStatePropertyType();
   }

   void ITweenController::InitWithPropsInstant(struct BaseStateProperty* dstStateProperty)
   {

   }
}
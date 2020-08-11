#include "IStateMachineController.h"
#include "StateProperty.h"
#include "Core/CommonCore/Assertion.h"

namespace Game {

   IStateMachineController::IStateMachineController()
   {
   }

   void IStateMachineController::OnTransitionStarted(BaseStateProperty* srcStateProperty, BaseStateProperty* dstStateProperty, const float transitionDuration)
   {
      mPropertyBinding = srcStateProperty->PropertyBinding;
      TranstionProperties[(int)StateType::SourceState] = srcStateProperty;
      TranstionProperties[(int)StateType::DestinationState] = dstStateProperty;
   }

   void IStateMachineController::OnTransitionUpdate(const float deltaTime, const float transitionParameter) { }

   StatePropertyType IStateMachineController::GetControllerPropertyType() const
   {
      BaseStateProperty* srcProp = TranstionProperties[(int)StateType::SourceState];

      assert(srcProp != nullptr);

      return srcProp->GetStatePropertyType();
   }

   void IStateMachineController::InitWithPropsInstant(struct BaseStateProperty* dstStateProperty)
   {

   }
}
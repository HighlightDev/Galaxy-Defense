#include "IStateMachineController.h"
#include "StateProperty.h"
#include "Core/CommonCore/Assertion.h"

namespace Game {

   void IStateMachineController::OnTransitionStarted(BaseStateProperty* srcState, BaseStateProperty* dstState, const float transitionDuration)
   {
      TranstionProperties[(int)StateType::SourceState] = srcState;
      TranstionProperties[(int)StateType::DestinationState] = dstState;
   }

   void IStateMachineController::OnTransitionUpdate(const float deltaTime) { }

   StatePropertyType IStateMachineController::GetControllerPropertyType() const
   {
      BaseStateProperty* srcProp = TranstionProperties[(int)StateType::SourceState];

      assert(srcProp != nullptr);

      return srcProp->GetStatePropertyType();
   }
}
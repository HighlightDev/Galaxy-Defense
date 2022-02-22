#include "StateTransition.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{
   StateTransition::StateTransition(std::shared_ptr<State> stateFrom, std::shared_ptr<State> stateDestination, float transitionDuration)
      : StateFrom(stateFrom)
      , StateDestination(stateDestination)
      , TransitionDuration(transitionDuration)
   {
      assert(stateFrom != nullptr);
      assert(stateDestination != nullptr);
   }
}
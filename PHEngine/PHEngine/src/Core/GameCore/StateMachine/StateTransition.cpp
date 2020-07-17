#include "StateTransition.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{
   StateTransition::StateTransition(State* stateFrom, State* stateDestination, float transitionDuration)
      : StateFrom(stateFrom)
      , StateDestination(stateDestination)
      , TransitionDuration(transitionDuration)
   {
      assert(StateFrom != nullptr);
      assert(StateDestination != nullptr);
   }
}
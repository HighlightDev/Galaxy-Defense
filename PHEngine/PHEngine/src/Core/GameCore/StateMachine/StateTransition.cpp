#include "StateTransition.h"

namespace Game
{
   StateTransition::StateTransition(State* stateFrom, State* stateTo)
      : StateFrom(stateFrom)
      , StateTo(stateTo)
   {

   }
}
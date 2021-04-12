#pragma once

#include <memory>

namespace Game
{
   class State;

   struct StateTransition
   {
      std::weak_ptr<State> StateFrom;
      std::weak_ptr<State> StateDestination;
      
      float TransitionDuration;

      StateTransition(std::shared_ptr<State> stateFrom, std::shared_ptr<State>stateDestination, float transitionDuration);
   };
}
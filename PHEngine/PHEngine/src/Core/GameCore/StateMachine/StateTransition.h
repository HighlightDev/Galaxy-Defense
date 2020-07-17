#pragma once

namespace Game
{
   struct StateTransition
   {
      class State* StateFrom = nullptr;
      class State* StateDestination = nullptr;
      
      float TransitionDuration;

      StateTransition(class State* stateFrom, class State* stateDestination, float transitionDuration);
   };
}
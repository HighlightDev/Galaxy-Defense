#pragma once

namespace Game
{
   struct StateTransition
   {
      class State* StateFrom = nullptr;
      class State* StateTo = nullptr;

      StateTransition(class State* stateFrom, class State* stateTo);
   };
}
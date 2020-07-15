#pragma once

#include "State.h"

namespace Game
{

   class StateMachine
   {
      /* At beginning we are here */
      State* mStateNodeInitRoot = nullptr;

      State* mCurrentStateNode = nullptr;

   public:

      StateMachine(State*& rootNode);

      ~StateMachine();

      void ChangeState(const std::string& dstStateName);

      State* GetCurrentState() const;
   };

}
#include "StateMachine.h"

#include <algorithm>

namespace Game 
{

   StateMachine::StateMachine(State*& rootNode) 
      : mStateNodeInitRoot(rootNode)
      , mCurrentStateNode(mStateNodeInitRoot)
   {
   }

   StateMachine::~StateMachine()
   {
      delete mStateNodeInitRoot;
   }

   void StateMachine::ChangeState(const std::string& dstStateName)
   {
      const std::map<std::string, StateTransition>& transitions = mCurrentStateNode->GetTransitions();

      if (transitions.count(dstStateName))
      {
         const StateTransition& transition = transitions.at(dstStateName);

      }

   }
}
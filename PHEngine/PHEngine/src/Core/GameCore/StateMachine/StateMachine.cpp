#include "StateMachine.h"
#include "Core/CommonCore/Assertion.h"

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

         assert(transition.StateFrom->GetStateName() == mCurrentStateNode->GetStateName());

         State* stateTo = transition.StateTo;

         std::map<std::string /*Property Name*/, BaseStateProperty*> currentProperties = mCurrentStateNode->GetStateProperties();
         std::map<std::string /*Property Name*/, BaseStateProperty*> dstProperties = stateTo->GetStateProperties();

         for (auto& currentNameAndPropertyPair : currentProperties)
         {
            const std::string& name = currentNameAndPropertyPair.first;

            if (dstProperties.count(name))
            {
               currentNameAndPropertyPair.second->ChangeState(dstProperties.at(name), /*Todo: for now it is 1.0 */1.0f);
            }
         }

         mCurrentStateNode = stateTo;
      }

   }

   State* StateMachine::GetCurrentState() const
   {
      return mCurrentStateNode;
   }
}
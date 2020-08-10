#include "StateMachine.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/StateMachine/AnimationStateMachineController.h"

#include <algorithm>

namespace Game 
{

   StateMachine::StateMachine(State*& rootNode) 
      : mStateNodeInitRoot(rootNode)
      , mCurrentStateNode(mStateNodeInitRoot)
   {
      MakeTransition(rootNode->GetStateName()); // set initial state
   }

   StateMachine::~StateMachine()
   {
      delete mStateNodeInitRoot;
   }

   void StateMachine::MakeTransition(const std::string& stateName)
   {
      const std::map<std::string /*dstStateName*/, StateTransition>& transitions = mCurrentStateNode->GetTransitions();

      if (transitions.count(stateName))
      {
         const StateTransition& transition = transitions.at(stateName);

         State* stateTo = transition.StateDestination;
         State* stateFrom = transition.StateFrom;

         assert(stateFrom->GetStateName() == mCurrentStateNode->GetStateName());

         mCurrentActiveStateTransition = &transition;
         mTransitionTime = 0.0f;
         mTransitionParameter = 0.0f;
         mTransitionDuration = transition.TransitionDuration;
         bTransitionEnabled = true;

         std::map<std::string /*Property Name*/, BaseStateProperty*> srcProperties = stateFrom->GetStateProperties();
         std::map<std::string /*Property Name*/, BaseStateProperty*> dstProperties = stateTo->GetStateProperties();

         for (auto& srcNameAndPropertyPair : srcProperties)
         {
            const std::string& name = srcNameAndPropertyPair.first;

            if (dstProperties.count(name))
            {
               BaseStateProperty* srcProperty = srcNameAndPropertyPair.second;
               BaseStateProperty* dstProperty = dstProperties[name];

               IStateMachineController* propertyController = nullptr;

               if (srcProperty->GetStatePropertyType() == StatePropertyType::Animation)
               {
                  propertyController = new AnimationStateMachineController();
               }

               if (propertyController)
               {
                  CurrentActiveTransitionControllers.emplace_back(std::shared_ptr<IStateMachineController>(propertyController));
                  propertyController->OnTransitionStarted(srcProperty, dstProperty, mTransitionDuration);
               }
            }
         }
      }
   }

   void StateMachine::ChangeState(const std::string& dstStateName)
   {
      if (bTransitionEnabled || mCurrentStateNode->GetStateName() == dstStateName)
         return;

      MakeTransition(dstStateName);
   }
   
   void StateMachine::Tick(const float deltaTime)
   {
      // process current transition
      if (bTransitionEnabled && mCurrentActiveStateTransition)
      {
         State* stateTo = mCurrentActiveStateTransition->StateDestination;
         State* stateFrom = mCurrentActiveStateTransition->StateFrom;

         mTransitionTime += deltaTime;

         if (mTransitionTime > mTransitionDuration)
         {
            mTransitionParameter = 1.0f;
            mTransitionTime = 0.0f;
            mCurrentStateNode = stateTo;
            mCurrentActiveStateTransition = nullptr;

            bTransitionEnabled = false;
         }
         else
         {
            mTransitionParameter = mTransitionTime / mTransitionDuration;
         }

         for (std::shared_ptr<IStateMachineController>& controllerSp : CurrentActiveTransitionControllers)
         {
            if (bTransitionEnabled)
            {
               controllerSp->OnTransitionUpdate(deltaTime, mTransitionParameter);
            }
            else
            {
               controllerSp->OnTransitionFinished();
            }
         }

         if (!bTransitionEnabled)
         {
            CurrentActiveTransitionControllers.clear();
         }
      }
   }

   bool StateMachine::IsTransitionActive() const
   {
      return bTransitionEnabled;
   }

   float StateMachine::GetTransitionParameter() const
   {
      return mTransitionParameter;
   }

   State* StateMachine::GetCurrentState() const
   {
      return mCurrentStateNode;
   }
}
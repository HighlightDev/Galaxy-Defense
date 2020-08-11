#include "StateMachine.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/StateMachine/AnimationStateMachineController.h"

#include <algorithm>
#include <iostream>

namespace Game 
{

   StateMachine::StateMachine(State*& rootNode) 
      : mStateNodeInitRoot(rootNode)
      , mCurrentStateNode(mStateNodeInitRoot)
   {
      InitRootState();
   }

   StateMachine::~StateMachine()
   {
      delete mStateNodeInitRoot;
   }

   void StateMachine::InitRootState()
   {
      const std::string& rootStateName = mStateNodeInitRoot->GetStateName();

      std::map<std::string /*Property Name*/, BaseStateProperty*> dstProperties = mStateNodeInitRoot->GetStateProperties();

      std::shared_ptr<IStateMachineController> propertyController;

      for (auto& dstNameAndPropertyPair : dstProperties)
      {
         BaseStateProperty* dstProperty = dstNameAndPropertyPair.second;
         if (dstProperty->GetStatePropertyType() == StatePropertyType::Animation)
         {
            propertyController = std::make_shared<AnimationStateMachineController>();
         }

         if (propertyController)
         {
            propertyController->InitWithPropsInstant(dstProperty);
         }
      }
   }

   void StateMachine::ChangeState(const std::string& dstStateName)
   {
      if (bTransitionEnabled || mCurrentStateNode->GetStateName() == dstStateName)
         return;

      std::cout << "Change state : " << dstStateName << std::endl;

      const std::map<std::string /*dstStateName*/, StateTransition>& transitions = mCurrentStateNode->GetTransitions();

      if (transitions.count(dstStateName))
      {
         const StateTransition& transition = transitions.at(dstStateName);

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
   
   void StateMachine::Tick(const float deltaTime)
   {
      // process current transition
      if (bTransitionEnabled && mCurrentActiveStateTransition)
      {
         State* stateTo = mCurrentActiveStateTransition->StateDestination;

         mTransitionTime += deltaTime * 10;

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
               std::cout << "Transition finished." << "Current state : " + mCurrentStateNode->GetStateName() << std::endl;
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
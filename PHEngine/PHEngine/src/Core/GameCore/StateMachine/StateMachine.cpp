#include "StateMachine.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/StateMachine/AnimationStateMachineController.h"
#include "Core/GameCore/StateMachine/FloatStateMachineController.h"

#include <algorithm>
#include <iostream>

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

   void StateMachine::InitRootState()
   {
      const std::string& rootStateName = mStateNodeInitRoot->GetStateName();

      std::map<std::string /*Property Name*/, BaseStateProperty*> dstProperties = mStateNodeInitRoot->GetStateProperties();

      for (auto& dstNameAndPropertyPair : dstProperties)
      {
         BaseStateProperty* dstProperty = dstNameAndPropertyPair.second;

         std::shared_ptr<IStateMachineController> propertyController;

         const auto propertyType = dstProperty->GetStatePropertyType();
         if (StatePropertyType::Animation == propertyType)
         {
            propertyController = std::make_shared<AnimationStateMachineController>();
         }
         else if (StatePropertyType::Float == propertyType)
         {
            propertyController = std::make_shared<FloatStateMachineController>();
         }

         if (propertyController)
         {
            propertyController->InitWithPropsInstant(dstProperty);
         }
      }
   }

   void StateMachine::DoTranstionInstantly(const std::string& dstStateName)
   {
      if (mCurrentActiveStateTransition)
      {
         // Finish current transition
         for (std::shared_ptr<IStateMachineController>& controllerSp : CurrentActiveTransitionControllers)
         {
            controllerSp->OnTransitionFinished();
         }

         SetTransitionValuesFinished(mCurrentActiveStateTransition->StateDestination);
         CurrentActiveTransitionControllers.clear();

         // Begin new transition
         DoTransition(dstStateName);
      }
   }

   void StateMachine::DoTransition(const std::string& dstStateName)
   {
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

               std::shared_ptr<IStateMachineController> propertyController;

               const auto propertyType = srcProperty->GetStatePropertyType();
               if (StatePropertyType::Animation == propertyType)
               {
                  propertyController = std::make_shared<AnimationStateMachineController>();
               }
               else if (StatePropertyType::Float == propertyType)
               {
                  propertyController = std::make_shared<FloatStateMachineController>();
               }

               if (propertyController)
               {
                  CurrentActiveTransitionControllers.emplace_back(propertyController);
                  propertyController->OnTransitionStarted(srcProperty, dstProperty, mTransitionDuration);
               }
            }
         }
      }
   }

   void StateMachine::ChangeState(const std::string& dstStateName)
   {
      if (bTransitionEnabled)
      {
         // Transition is not finished yet, but state should be already changed
         DoTranstionInstantly(dstStateName);
      }
      else
      {
         DoTransition(dstStateName);
      }
   }

   void StateMachine::SetTransitionValuesFinished(State* newCurrentState)
   {
      std::cout << "TRANSITION FINISHED, new STATE : " + newCurrentState->GetStateName() << std::endl;
      mTransitionParameter = 1.0f;
      mTransitionTime = 0.0f;
      mCurrentStateNode = newCurrentState;
      mCurrentActiveStateTransition = nullptr;
      bTransitionEnabled = false;
   }

   std::shared_ptr<StatePropertyBinding> StateMachine::GetPropertyBindingByName(const std::string& name) const
   {
      assert(mPropertyBindings.count(name));
      return mPropertyBindings.at(name);
   }

   void StateMachine::AddPropertyBinding(const std::string& propBindingName, std::shared_ptr<StatePropertyBinding> binding)
   {
      assert(binding);
      mPropertyBindings[propBindingName] = binding;
   }

   void StateMachine::Tick(const float deltaTime)
   {
      // process current transition
      if (bTransitionEnabled && mCurrentActiveStateTransition)
      {
         State* stateTo = mCurrentActiveStateTransition->StateDestination;

         mTransitionTime += deltaTime * 5.0f;

         if (mTransitionTime > mTransitionDuration)
         {
            SetTransitionValuesFinished(stateTo);
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
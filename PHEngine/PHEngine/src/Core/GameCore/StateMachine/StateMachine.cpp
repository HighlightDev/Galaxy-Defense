#include "StateMachine.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/StateMachine/AnimationStateMachineController.h"
#include "Core/GameCore/StateMachine/FloatStateMachineController.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"
#include "Core/GameCore/Actor.h"

#include <algorithm>
#include <iostream>

namespace Game
{

   StateMachine::StateMachine(const std::string& relPathFSM, std::shared_ptr<State> rootNode)
      : mRelPathFSM(relPathFSM)
      , mStateNodeInitRoot(rootNode)
      , mCurrentStateNode(mStateNodeInitRoot)
   {
   }

   StateMachine::~StateMachine()
   {
   }

   void StateMachine::InitRootState()
   {
      if (auto spInitNode = mStateNodeInitRoot.lock())
      {
         const std::string& rootStateName = spInitNode->GetStateName();

         std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> dstProperties = spInitNode->GetStateProperties();

         for (auto& dstNameAndPropertyPair : dstProperties)
         {
            std::shared_ptr<BaseStateProperty> dstProperty = dstNameAndPropertyPair.second;

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
               propertyController->InitWithPropsInstant(dstProperty.get());
            }
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

         if (auto spDestination = mCurrentActiveStateTransition->StateDestination.lock())
         {
            SetTransitionValuesFinished(spDestination);
            CurrentActiveTransitionControllers.clear();

            // Begin new transition
            DoTransition(dstStateName);
         }
      }
   }

   void StateMachine::DoTransition(const std::string& dstStateName)
   {
      if (auto spCurrentNode = mCurrentStateNode.lock())
      {
         const std::map<std::string /*dstStateName*/, StateTransition>& transitions = spCurrentNode->GetTransitions();

         if (transitions.count(dstStateName))
         {
            const StateTransition& transition = transitions.at(dstStateName);

            auto spDestination = transition.StateDestination.lock();
            auto spFrom = transition.StateFrom.lock();

            if (spDestination && spFrom)
            {
               State* stateTo = spDestination.get();
               State* stateFrom = spFrom.get();

               assert(stateFrom->GetStateName() == spCurrentNode->GetStateName());

               mCurrentActiveStateTransition = &transition;
               mTransitionTime = 0.0f;
               mTransitionParameter = 0.0f;
               mTransitionDuration = transition.TransitionDuration;
               bTransitionEnabled = true;

               std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> srcProperties = stateFrom->GetStateProperties();
               std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> dstProperties = stateTo->GetStateProperties();

               for (auto& srcNameAndPropertyPair : srcProperties)
               {
                  const std::string& name = srcNameAndPropertyPair.first;

                  if (dstProperties.count(name))
                  {
                     std::shared_ptr<BaseStateProperty> srcProperty = srcNameAndPropertyPair.second;
                     std::shared_ptr<BaseStateProperty> dstProperty = dstProperties[name];

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
                        propertyController->OnTransitionStarted(srcProperty.get(), dstProperty.get(), mTransitionDuration);
                     }
                  }
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

   void StateMachine::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto it = std::find_if(dataContainer.Actors.begin(), dataContainer.Actors.end(), [=](const SerializeDataActor& actorData) { return actorData.ActorName == GetParentActor()->GetName(); });
      assert(it != dataContainer.Actors.end());

      std::shared_ptr<SerializeDataStateMachine> fsmData = std::make_shared<SerializeDataStateMachine>();

      fsmData->FsmRelPath = GetRelPathFSM();

      for (const auto& binding : mPropertyBindings)
      {
         SerializeDataStateMachine::SerializeFSMBinding bindingData;
         bindingData.BindingName = binding.second->BindingName;
         bindingData.GameObjectName = binding.second->GameObjectName;
         bindingData.GameObjectPropertyName = binding.second->GameObjectPropertyName;
         fsmData->Bindings.emplace_back(bindingData);
      }
      
      it->StateMachineData = fsmData;
   }

   void StateMachine::SetParentActor(Actor* parent) {
      mParent = parent;
   }

   Actor* StateMachine::GetParentActor() const {
      return mParent;
   }

   void StateMachine::SetTransitionValuesFinished(std::shared_ptr<State> newCurrentState)
   {
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
         if (auto spDestination = mCurrentActiveStateTransition->StateDestination.lock())
         {
            std::shared_ptr<State> stateTo = spDestination;

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
   }

   bool StateMachine::IsTransitionActive() const
   {
      return bTransitionEnabled;
   }

   float StateMachine::GetTransitionParameter() const
   {
      return mTransitionParameter;
   }

   std::shared_ptr<State> StateMachine::GetCurrentState() const
   {
      return mCurrentStateNode.lock();
   }

   std::string StateMachine::GetRelPathFSM() const {
      return mRelPathFSM;
   }
}
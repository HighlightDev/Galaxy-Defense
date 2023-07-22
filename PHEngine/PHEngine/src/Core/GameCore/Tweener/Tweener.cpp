#include "Tweener.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Tweener/AnimationTweenController.h"
#include "Core/GameCore/Tweener/FloatTweenController.h"
#include "Core/GameCore/Tweener/EulerAnglesRotationTweenController.h"
#include "Core/GameCore/Tweener/BooleanTweenController.h"
#include "Core/GameCore/Tweener/Vec3TweenController.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/LoggerExtension.h"

#include <algorithm>

namespace EngineCore
{
   int32_t Tweener::s_id = 0;

   Tweener::Tweener(const std::string &relPathFSM, const std::string &tweenerInnerName, std::shared_ptr<State> rootNode, std::vector<std::shared_ptr<State>> &&allStates)
       : m_id(s_id++),
         mMyAllStates(std::move(allStates)),
         mRelPathTweener(relPathFSM),
         mTweenerName(tweenerInnerName),
         mStateNodeInitRoot(rootNode),
         mCurrentStateNode(mStateNodeInitRoot),
         bIsStateChangedDirty(false),
         mChangedStateName("")
   {
   }

   int32_t Tweener::GetId() const
   {
      return m_id;
   }

   std::shared_ptr<ITweenController> GetPropertyTweenerController(const eEnginePropertyBindingType propertyType)
   {
      std::shared_ptr<ITweenController> propertyController;

      if (eEnginePropertyBindingType::Animation == propertyType)
      {
         propertyController = std::make_shared<AnimationTweenController>();
      }
      else if (eEnginePropertyBindingType::FloatScalar == propertyType)
      {
         propertyController = std::make_shared<FloatTweenController>();
      }
      else if (eEnginePropertyBindingType::EulerAnglesRotation == propertyType)
      {
         propertyController = std::make_shared<EulerAnglesRotationTweenController>();
      }
      else if (eEnginePropertyBindingType::Boolean == propertyType)
      {
         propertyController = std::make_shared<BooleanTweenController>();
      }
      else if (eEnginePropertyBindingType::Vec3 == propertyType)
      {
         propertyController = std::make_shared<Vec3TweenController>();
      }

      assert(propertyController);

      return propertyController;
   }

   void Tweener::InitRootState()
   {
      const std::string &rootStateName = mStateNodeInitRoot->GetStateName();

      std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> dstProperties = mStateNodeInitRoot->GetStateProperties();

      for (auto &dstNameAndPropertyPair : dstProperties)
      {
         std::shared_ptr<BaseStateProperty> dstProperty = dstNameAndPropertyPair.second;

         const auto propertyController = GetPropertyTweenerController(dstProperty->GetStatePropertyType());
         propertyController->InitWithPropsInstant(dstProperty);
      }

      SetTransitionValuesFinished(mStateNodeInitRoot);
   }

   void Tweener::DoTranstionInstantly(const std::string &dstStateName)
   {
      if (mCurrentActiveStateTransition.has_value())
      {
         for (std::shared_ptr<ITweenController> &controllerSp : CurrentActiveTransitionControllers)
         {
            controllerSp->OnTransitionFinished();
         }

         CurrentActiveTransitionControllers.clear();

         auto dstStateIt = std::find_if(mMyAllStates.begin(), mMyAllStates.end(), [=](const auto &state)
                                        { return state->GetStateName() == dstStateName; });
         assert(dstStateIt != mMyAllStates.end());
         auto dstStateSp = (*dstStateIt);

         std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> dstProperties = dstStateSp->GetStateProperties();
         for (auto &dstNameAndPropertyPair : dstProperties)
         {
            const std::shared_ptr<BaseStateProperty> &dstProperty = dstNameAndPropertyPair.second;

            const auto propertyController = GetPropertyTweenerController(dstProperty->GetStatePropertyType());
            propertyController->InitWithPropsInstant(dstProperty);
         }

         SetTransitionValuesFinished(dstStateSp);
      }
   }

   void Tweener::DoTransition(const std::string &dstStateName)
   {
      const std::map<std::string /*dstStateName*/, StateTransition> &transitions = mCurrentStateNode->GetTransitions();

      assert(transitions.count(dstStateName));

      const StateTransition &transition = transitions.at(dstStateName);

      auto spDestination = transition.StateDestination.lock();
      auto spFrom = transition.StateFrom.lock();

      if (spDestination && spFrom)
      {
         assert(spFrom->GetStateName() == mCurrentStateNode->GetStateName());

         mCurrentActiveStateTransition = transition;
         mTransitionTime = 0.0f;
         mTransitionParameter = 0.0f;
         mTransitionDuration = transition.TransitionDuration;
         bTransitionEnabled = true;

         const std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> &srcProperties = spFrom->GetStateProperties();
         const std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> &dstProperties = spDestination->GetStateProperties();

         for (auto &dstNameAndPropertyPair : dstProperties)
         {
            const std::string &name = dstNameAndPropertyPair.first;
            assert(srcProperties.count(name)); // missing transition from src to dst property, probably forgot to add property state to src state

            const std::shared_ptr<BaseStateProperty> srcProperty = srcProperties.at(name);
            const std::shared_ptr<BaseStateProperty> dstProperty = dstNameAndPropertyPair.second;

            const auto propertyController = GetPropertyTweenerController(dstProperty->GetStatePropertyType());
            CurrentActiveTransitionControllers.emplace_back(propertyController);
            propertyController->OnTransitionStarted(srcProperty, dstProperty, mTransitionDuration);
         }
      }
   }

   void Tweener::ChangeState(const std::string &dstStateName)
   {
      if (bTransitionEnabled)
      {
         // Transition is not finished yet, but state should be already changed
         DoTranstionInstantly(dstStateName);
      }
      else
      {
         if (mCurrentStateNode->GetStateName() != dstStateName)
         {
            DoTransition(dstStateName);
         }
      }
   }

   void Tweener::CollectDataForSerialization(SerializeDataContainer &dataContainer)
   {
      const auto parentSp = GetParentActorWp().lock();
      assert(parentSp);
      auto it = std::find_if(dataContainer.Actors.begin(), dataContainer.Actors.end(), [parentName = parentSp->GetName()](const SerializeDataActor &actorData)
                             { return actorData.ActorName == parentName; });
      assert(it != dataContainer.Actors.end());

      std::shared_ptr<SerializeDataTweener> tweenerData = std::make_shared<SerializeDataTweener>();

      tweenerData->TweenerRelPath = GetRelPathTweener();

      for (const auto &binding : mPropertyBindings)
      {
         SerializeDataTweener::SerializeTweenerBinding bindingData;
         bindingData.BindingName = binding.second->BindingName;
         bindingData.EngineObjectName = binding.second->EngineObjectName;
         bindingData.EngineObjectPropertyName = binding.second->EngineObjectPropertyName;
         tweenerData->Bindings.emplace_back(bindingData);
      }

      it->TweenerData = tweenerData;
   }

   void Tweener::SubscribeOnStateChange(const std::shared_ptr<ITweenStateChangeNotifyable> &observer)
   {
      mStateChangedObservers.emplace_back(observer);
   }

   void Tweener::SetParentActor(const std::shared_ptr<Actor> &parent)
   {
      mParentWp = parent;
   }

   std::weak_ptr<Actor> Tweener::GetParentActorWp() const
   {
      return mParentWp;
   }

   void Tweener::SetTransitionValuesFinished(std::shared_ptr<State> newCurrentState)
   {
      mTransitionParameter = 1.0f;
      mTransitionTime = 0.0f;
      mCurrentStateNode = newCurrentState;
      mCurrentActiveStateTransition = std::nullopt;
      bTransitionEnabled = false;
      mChangedStateName = newCurrentState->GetStateName();
      bIsStateChangedDirty = true;
   }

   void Tweener::NotifyStateChangedObservers()
   {
      if (bIsStateChangedDirty)
      {
         bIsStateChangedDirty = false;
         for (const auto &observerWp : mStateChangedObservers)
         {
            if (const auto &observerSp = observerWp.lock())
            {
               observerSp->OnTweenStateChanged(mChangedStateName);
            }
         }
      }
   }

   void Tweener::CleanUp()
   {
      LogInfo("Tweener::CleanUp => name: ", mTweenerName);
      mMyAllStates.clear();
      mPropertyBindings.clear();
      CurrentActiveTransitionControllers.clear();
      mStateChangedObservers.clear();
   }

   std::shared_ptr<PropertyBinding> Tweener::GetPropertyBindingByName(const std::string &name) const
   {
      assert(mPropertyBindings.count(name));
      return mPropertyBindings.at(name);
   }

   void Tweener::AddPropertyBinding(const std::string &propBindingName, std::shared_ptr<PropertyBinding> binding)
   {
      assert(binding);
      mPropertyBindings[propBindingName] = binding;
   }

   void Tweener::Tick(const float deltaTime)
   {
      // process current transition
      if (bTransitionEnabled && mCurrentActiveStateTransition.has_value())
      {
         if (auto spDestination = mCurrentActiveStateTransition->StateDestination.lock())
         {
            std::shared_ptr<State> stateTo = spDestination;

            mTransitionTime += deltaTime;

            if (mTransitionTime > mTransitionDuration)
            {
               SetTransitionValuesFinished(stateTo);
            }
            else
            {
               mTransitionParameter = mTransitionTime / mTransitionDuration;
            }

            for (const auto &controllerSp : CurrentActiveTransitionControllers)
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

   bool Tweener::IsTransitionActive() const
   {
      return bTransitionEnabled;
   }

   float Tweener::GetTransitionParameter() const
   {
      return mTransitionParameter;
   }

   std::shared_ptr<State> Tweener::GetCurrentState() const
   {
      return mCurrentStateNode;
   }

   std::string Tweener::GetRelPathTweener() const
   {
      return mRelPathTweener;
   }

   const std::string &Tweener::GetTweenerName() const
   {
      return mTweenerName;
   }
}
#pragma once

#include "State.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Serialize/ISerializable.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GameCore/Tweener/ITweenController.h"
#include "Core/GameCore/Tweener/ITweenStateChangeNotifyable.h"

#include <unordered_map>
#include <optional>

namespace EngineCore
{
   class Actor;

   class Tweener 
      : public ITickable
      , public ISerializable
   {
      std::vector<std::shared_ptr<State>> mMyAllStates;

      std::weak_ptr<Actor> mParentWp;

      std::string mTweenerName;
      
      std::string mRelPathTweener;

      /* At beginning we are here */
      std::shared_ptr<State> mStateNodeInitRoot;

      std::shared_ptr<State> mCurrentStateNode;

      std::optional<StateTransition> mCurrentActiveStateTransition{std::nullopt};

      std::unordered_map<std::string/*name of binding property*/, std::shared_ptr<PropertyBinding>> mPropertyBindings;

      std::vector<std::shared_ptr<ITweenController>> CurrentActiveTransitionControllers;

      bool bTransitionEnabled = false;
      /* this parameter is mapped from 0.0 (start of transition) to 1.0 (end of transition) */
      float mTransitionParameter = 0.0f;
      float mTransitionTime = 0.0f;
      float mTransitionDuration = 0.0f;

      bool bIsStateChangedDirty;
      std::string mChangedStateName;
      std::vector<ITweenStateChangeNotifyable*> mStateChangedObservers; 
   public:

      Tweener(const std::string& relPathFSM, const std::string& tweenerInnerName, std::shared_ptr<State> rootNode, std::vector<std::shared_ptr<State>> &&allStates);

      ~Tweener();

      void ChangeState(const std::string& dstStateName);

      void Tick(const float deltaTime) override;

      void UnpausableTick(const float deltaTime) override {};

      void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      void SubscribeOnStateChange(ITweenStateChangeNotifyable* observer);

      std::shared_ptr<State> GetCurrentState() const;

      bool IsTransitionActive() const;

      float GetTransitionParameter() const;

      std::string GetRelPathTweener() const;

      const std::string& GetTweenerName() const;

      void AddPropertyBinding(const std::string& propBindingName, std::shared_ptr<PropertyBinding> binding);

      std::shared_ptr<PropertyBinding> GetPropertyBindingByName(const std::string& name) const;

      void InitRootState();

      void SetParentActor(const std::shared_ptr<Actor>& parent);

      std::weak_ptr<Actor> GetParentActorWp() const;

      void NotifyStateChangedObservers();

      void CleanUp();

   private:

      void DoTransition(const std::string& dstStateName);
      
      void DoTranstionInstantly(const std::string& dstStateName);

      void SetTransitionValuesFinished(std::shared_ptr<State> newCurrentState);
   };

}
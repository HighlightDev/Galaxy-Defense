#pragma once

#include "State.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Serialize/ISerializable.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"
#include "Core/GameCore/Tweener/ITweenController.h"

#include <unordered_map>

namespace EngineCore
{
   class Tweener 
      : public ITickable
      , public ISerializable
   {
      std::vector<std::shared_ptr<State>> mMyAllStates;

      class Actor* mParent = nullptr;

      std::string mRelPathTweener;

      /* At beginning we are here */
      std::shared_ptr<State> mStateNodeInitRoot;

      std::shared_ptr<State> mCurrentStateNode;

      const StateTransition* mCurrentActiveStateTransition = nullptr;

      std::unordered_map<std::string/*name of binding property*/, std::shared_ptr<PropertyBinding>> mPropertyBindings;

      std::vector<std::shared_ptr<ITweenController>> CurrentActiveTransitionControllers;

      bool bTransitionEnabled = false;
      /* this parameter is mapped from 0.0 (start of transition) to 1.0 (end of transition) */
      float mTransitionParameter = 0.0f;
      float mTransitionTime = 0.0f;
      float mTransitionDuration = 0.0f;

   public:

      Tweener(const std::string& relPathFSM, std::shared_ptr<State> rootNode, std::vector<std::shared_ptr<State>> allStates);

      ~Tweener();

      void ChangeState(const std::string& dstStateName);

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      std::shared_ptr<State> GetCurrentState() const;

      bool IsTransitionActive() const;

      float GetTransitionParameter() const;

      std::string GetRelPathTweener() const;

      void AddPropertyBinding(const std::string& propBindingName, std::shared_ptr<PropertyBinding> binding);

      std::shared_ptr<PropertyBinding> GetPropertyBindingByName(const std::string& name) const;

      void InitRootState();

      void SetParentActor(class Actor* parent);

      class Actor* GetParentActor() const;

   private:

      void DoTransition(const std::string& dstStateName);
      
      void DoTranstionInstantly(const std::string& dstStateName);

      void SetTransitionValuesFinished(std::shared_ptr<State> newCurrentState);
   };

}
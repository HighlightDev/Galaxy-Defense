#pragma once

#include "State.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Serialize/ISerializable.h"
#include "Core/GameCore/Serialize/SerializeData/SerializeDataContainer.h"

#include <unordered_map>

namespace Game
{
   class StateMachine 
      : public ITickable
      , public ISerializable
   {
      class Actor* mParent = nullptr;

      std::string mRelPathFSM;

      /* At beginning we are here */
      State* mStateNodeInitRoot = nullptr;

      State* mCurrentStateNode = nullptr;

      const StateTransition* mCurrentActiveStateTransition = nullptr;

      std::unordered_map<std::string/*name of binding property*/, std::shared_ptr<StatePropertyBinding>> mPropertyBindings;

      std::vector<std::shared_ptr<IStateMachineController>> CurrentActiveTransitionControllers;

      bool bTransitionEnabled = false;
      /* this parameter is mapped from 0.0 (start of transition) to 1.0 (end of transition) */
      float mTransitionParameter = 0.0f;
      float mTransitionTime = 0.0f;
      float mTransitionDuration = 0.0f;

   public:

      StateMachine(const std::string& relPathFSM, State*& rootNode);

      ~StateMachine();

      void ChangeState(const std::string& dstStateName);

      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      State* GetCurrentState() const;

      bool IsTransitionActive() const;

      float GetTransitionParameter() const;

      std::string GetRelPathFSM() const;

      void AddPropertyBinding(const std::string& propBindingName, std::shared_ptr<StatePropertyBinding> binding);

      std::shared_ptr<StatePropertyBinding> GetPropertyBindingByName(const std::string& name) const;

      void InitRootState();

      void SetParentActor(class Actor* parent);

      class Actor* GetParentActor() const;

   private:

      void DoTransition(const std::string& dstStateName);
      
      void DoTranstionInstantly(const std::string& dstStateName);

      void SetTransitionValuesFinished(State* newCurrentState);
   };

}
#pragma once

#include "State.h"

namespace Game
{

   class StateMachine
   {
      /* At beginning we are here */
      State* mStateNodeInitRoot = nullptr;

      State* mCurrentStateNode = nullptr;

      const StateTransition* mCurrentActiveStateTransition = nullptr;

      std::vector<std::shared_ptr<IStateMachineController>> CurrentActiveTransitionControllers;

      bool bTransitionEnabled = false;
      /* this parameter is mapped from 0.0 (start of transition) to 1.0 (end of transition) */
      float mTransitionParameter = 0.0f;
      float mTransitionTime = 0.0f;
      float mTransitionDuration = 0.0f;

   public:

      StateMachine(State*& rootNode);

      ~StateMachine();

      void ChangeState(const std::string& dstStateName);

      void Tick(const float deltaTime);

      State* GetCurrentState() const;

      bool IsTransitionActive() const;

      float GetTransitionParameter() const;

   private:

      void InitRootState();

      void DoTransition(const std::string& dstStateName);
      
      void DoTranstionInstantly(const std::string& dstStateName);

      void SetTransitionValuesFinished(State* newCurrentState);
   };

}
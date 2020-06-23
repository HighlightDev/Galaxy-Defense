#pragma once
#include "BaseState.h"

namespace Game
{
   class IStateMachineController
   {

      float mTransitionTime = 0.0f;

      float mTransitionDuration;
      
   protected:

      bool bTransitionEnabled = false;

      /* this parameter is mapped from 0.0 (start of transition) to 1.0 (end of transition) */
      float transitionParameter = 0.0f;

   public:
      
      virtual void OnTransitionFinished() = 0;

      virtual void MakeTransitionToState(BaseState& srcState, BaseState& dstState, const float transitionDuration)
      {
         bTransitionEnabled = true;
         mTransitionDuration = transitionDuration;
      }

      void UpdateTransitionTime(const float deltaTime)
      {
         if (bTransitionEnabled)
         {
            mTransitionTime += deltaTime;

            if (mTransitionTime > mTransitionDuration)
            {
               bTransitionEnabled = false;
               transitionParameter = 1.0f;
               mTransitionTime = 0.0f;
               OnTransitionFinished();
            }
            else
            {
               transitionParameter = mTransitionTime / mTransitionDuration;
            }
         }
      }

   };
}
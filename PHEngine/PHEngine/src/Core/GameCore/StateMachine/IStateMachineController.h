#pragma once

namespace Game
{
   enum class StateType
   {
      SourceState = 0,
      DestinationState = 1
   };

   class IStateMachineController
   {
   protected:

      struct BaseStateProperty* TranstionProperties[2];

   public:

      virtual void OnTransitionFinished() = 0;

      virtual void OnTransitionStarted(struct BaseStateProperty* srcState, struct BaseStateProperty* dstState, const float transitionDuration);

      virtual void OnTransitionUpdate(const float deltaTime);

      enum class StatePropertyType GetControllerPropertyType() const;
   };
}
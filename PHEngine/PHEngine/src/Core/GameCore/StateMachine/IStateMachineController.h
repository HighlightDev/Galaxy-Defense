#pragma once
#include "StatePropertyBinding.h"

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

      std::weak_ptr<StatePropertyBinding> mPropertyBinding;

   public:

      IStateMachineController();

      virtual ~IStateMachineController();

      virtual void OnTransitionFinished() = 0;

      virtual void OnTransitionStarted(struct BaseStateProperty* srcStateProperty, struct BaseStateProperty* dstStateProperty, const float transitionDuration);

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter);

      virtual void InitWithPropsInstant(struct BaseStateProperty* dstStateProperty);

      enum class StatePropertyType GetControllerPropertyType() const;
   };
}
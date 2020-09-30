#pragma once
#include "IStateMachineController.h"
#include "StatePropertyBinding.h"
#include "StateProperty.h"

namespace Game
{

   class FloatStateMachineController :
      public IStateMachineController
   {
      using Base = IStateMachineController;

   public:
      FloatStateMachineController();
      virtual ~FloatStateMachineController();

      virtual void OnTransitionStarted(struct BaseStateProperty* srcState, struct BaseStateProperty* dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      virtual void InitWithPropsInstant(struct BaseStateProperty* dstStateProperty) override;

   private:

      std::shared_ptr<FloatPropertyBinding> GetFloatPropertyBindingSP() const;
   };

}


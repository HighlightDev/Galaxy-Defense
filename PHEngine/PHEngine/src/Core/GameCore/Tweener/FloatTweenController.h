#pragma once
#include "ITweenController.h"
#include "StatePropertyBinding.h"
#include "StateProperty.h"

namespace Game
{

   class FloatTweenController :
      public ITweenController
   {
      using Base = ITweenController;

   public:
      FloatTweenController();
      virtual ~FloatTweenController();

      virtual void OnTransitionStarted(struct BaseStateProperty* srcState, struct BaseStateProperty* dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      virtual void InitWithPropsInstant(struct BaseStateProperty* dstStateProperty) override;

   private:

      std::shared_ptr<FloatPropertyBinding> GetFloatPropertyBindingSP() const;
   };

}


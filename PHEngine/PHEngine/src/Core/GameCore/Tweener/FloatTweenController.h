#pragma once
#include "ITweenController.h"
#include "StateProperty.h"

namespace EngineCore
{

   class FloatTweenController :
      public ITweenController
   {
      using Base = ITweenController;
      using TweenStateProperty_t = StateProperty<eBindingType::FloatScalar>;

   public:
      FloatTweenController();
      virtual ~FloatTweenController();

      virtual void OnTransitionStarted(const std::shared_ptr<BaseStateProperty>& srcState, const std::shared_ptr<BaseStateProperty>& dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      virtual void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty) override;

   private:

      std::shared_ptr<FloatPropertyBinding> GetFloatPropertyBindingSP() const;
   };

}


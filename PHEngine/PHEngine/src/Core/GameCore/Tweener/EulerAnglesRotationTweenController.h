#pragma once
#include "ITweenController.h"
#include "StateProperty.h"

namespace EngineCore
{

   class EulerAnglesRotationTweenController :
      public ITweenController
   {
      using Base = ITweenController;
      using TweenStateProperty_t = StateProperty<eBindingType::EulerAnglesRotation>;

   public:
      EulerAnglesRotationTweenController();
      virtual ~EulerAnglesRotationTweenController();

      virtual void OnTransitionStarted(BaseStateProperty* srcState, BaseStateProperty* dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      virtual void InitWithPropsInstant(BaseStateProperty* dstStateProperty) override;

   private:

      std::shared_ptr<EulerAnglesRotationPropertyBinding> GetRotationPropertyBindingSP() const;
   };

}


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

      void OnTransitionStarted(const std::shared_ptr<BaseStateProperty>& srcState, const std::shared_ptr<BaseStateProperty>& dstState, const float duration) override;

      void OnTransitionFinished() override;

      void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty) override;

   private:

      std::shared_ptr<EulerAnglesRotationPropertyBinding> GetRotationPropertyBindingSP() const;
   };

}


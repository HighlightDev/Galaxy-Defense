#pragma once
#include "ITweenController.h"
#include "StateProperty.h"
#include "Core/GameCore/GameObjectPropertyBindings/BooleanPropertyBinding.h"

namespace EngineCore
{

   class BooleanTweenController :
      public ITweenController
   {
      using Base = ITweenController;
      using TweenStateProperty_t = StateProperty<eBindingType::Boolean>;

   public:
      BooleanTweenController();
      virtual ~BooleanTweenController();

      virtual void OnTransitionStarted(const std::shared_ptr<BaseStateProperty>& srcState, const std::shared_ptr<BaseStateProperty>& dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      virtual void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty) override;

   private:

      std::shared_ptr<BooleanPropertyBinding> GetBooleanPropertyBindingSP() const;
   };

}


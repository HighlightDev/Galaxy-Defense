#pragma once
#include "ITweenController.h"
#include "StateProperty.h"
#include "Core/GameCore/EngineObjectPropertyBindings/BooleanPropertyBinding.h"

namespace EngineCore
{

   class BooleanTweenController :
      public ITweenController
   {
      using Base = ITweenController;
      using TweenStateProperty_t = StateProperty<eEnginePropertyBindingType::Boolean>;

   public:
      BooleanTweenController();
      
      virtual ~BooleanTweenController();

      void OnTransitionStarted(const std::shared_ptr<BaseStateProperty>& srcState, const std::shared_ptr<BaseStateProperty>& dstState, const float duration) override;

      void OnTransitionFinished() override;

      void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty) override;

   private:

      std::shared_ptr<BooleanPropertyBinding> GetBooleanPropertyBindingSP() const;
   };

}


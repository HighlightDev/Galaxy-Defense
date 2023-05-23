#pragma once
#include "ITweenController.h"
#include "StateProperty.h"

namespace EngineCore
{

   class Vec3TweenController :
      public ITweenController
   {
      using Base = ITweenController;
      using TweenStateProperty_t = StateProperty<eEnginePropertyBindingType::Vec3>;
      

   public:
      Vec3TweenController();
      
      virtual ~Vec3TweenController();

      void OnTransitionStarted(const std::shared_ptr<BaseStateProperty>& srcState, const std::shared_ptr<BaseStateProperty>& dstState, const float duration) override;

      void OnTransitionFinished() override;

      void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty) override;

   private:

      std::shared_ptr<Vec3PropertyBinding> GetVec3PropertyBindingSP() const;
   };

}


#pragma once

#include "ITweenController.h"
#include "Core/GraphicsCore/Mesh/AnimationPlayer.h"
#include "Core/GameCore/GameObjectPropertyBindings/AnimationPropertyBinding.h"
#include "StateProperty.h"

using namespace Graphics::Mesh;

namespace EngineCore
{
   struct BaseStateProperty;

   class AnimationTweenController
      : public ITweenController
   {
      using Base = ITweenController;
      using TweenStateProperty_t = StateProperty<eBindingType::Animation>;

   public:

      AnimationTweenController();

      virtual void OnTransitionStarted(const std::shared_ptr<BaseStateProperty>&, const std::shared_ptr<BaseStateProperty>& dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      virtual void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty) override;

   private:

      std::shared_ptr<AnimationPropertyBinding> GetAnimationPropertyBindingSP() const;
   };

}

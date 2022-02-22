#pragma once

#include "ITweenController.h"
#include "Core/GraphicsCore/Mesh/AnimationPlayer.h"
#include "Core/GameCore/GameObjectPropertyBindings/AnimationPropertyBinding.h"

using namespace Graphics::Mesh;

namespace EngineCore
{

   class AnimationTweenController
      : public ITweenController
   {
      using Base = ITweenController;

   public:

      AnimationTweenController();

      virtual void OnTransitionStarted(struct BaseStateProperty* srcState, struct BaseStateProperty* dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      virtual void InitWithPropsInstant(struct BaseStateProperty* dstStateProperty) override;

   private:

      std::shared_ptr<AnimationPropertyBinding> GetAnimationPropertyBindingSP() const;
   };

}

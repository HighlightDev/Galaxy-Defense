#pragma once

#include "IStateMachineController.h"
#include "Core/GraphicsCore/Mesh/AnimationPlayer.h"

using namespace Graphics::Mesh;

namespace Game
{

   class AnimationStateMachineController
      : public IStateMachineController
   {
      using Base = IStateMachineController;

   public:

      AnimationStateMachineController();

      virtual void OnTransitionStarted(struct BaseStateProperty* srcState, struct BaseStateProperty* dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime, const float transitionParameter) override;

      virtual void InitWithPropsInstant(struct BaseStateProperty* dstStateProperty) override;

   private:

      std::shared_ptr<AnimationPropertyBinding> GetAnimationPropertyBindingSP() const;
   };

}

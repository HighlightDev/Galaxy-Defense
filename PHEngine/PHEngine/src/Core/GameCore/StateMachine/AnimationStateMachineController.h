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

      std::weak_ptr<AnimationPlayer> mAnimationPlayer;

   public:

      AnimationStateMachineController(std::weak_ptr<AnimationPlayer> animationPlayer);

      virtual void MakeTransitionToState(struct BaseStateProperty* srcState, struct BaseStateProperty* dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void UpdateTransitionTime(const float deltaTime) override;
   };

}

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

      /* this is the main animation time counter*/
      float mSrcAnimationTime;

      /* this time is used when blending of animations is being calculated*/
      float mDstAnimationTime;

      /* this is the main animation name*/
      std::string mSrcAnimationName;

      /* this animation name is used when blending of animations is being occurred*/
      std::string mDstAnimationName;

   public:

      AnimationStateMachineController();

      virtual void OnTransitionStarted(struct BaseStateProperty* srcState, struct BaseStateProperty* dstState, const float duration) override;

      virtual void OnTransitionFinished() override;

      virtual void OnTransitionUpdate(const float deltaTime) override;

      float GetSrcAnimationTime() const;

      float GetDstAnimationTime() const;

      std::string GetSrcAnimationName() const;

      std::string GetDstAnimationName() const;
   };

}

#pragma once

#include "AnimatedMeshData.h"
#include "Core/GameCore/StateMachine/IStateMachineController.h"

using namespace Game;

namespace Graphics
{
   namespace Mesh
   {

      class AnimationPlayer 
         : public IStateMachineController
      {
         std::shared_ptr<AnimatedMeshData> m_animatedMeshData;

         /* this is the main animation time counter*/
         float mSrcAnimationTime;

         /* this time is used when blending of animations is being calculated*/
         float mDstAnimationTime;

         /* this is the main animation name*/
         std::string mSrcAnimationName;

         /* this animation name is used when blending of animations is being occurred*/
         std::string mDstAnimationName;

      public:

         AnimationPlayer(std::shared_ptr<AnimatedMeshData> animatedData);

         AnimationPlayer() = default;

         void UpdateAnimationTime(const float deltaTime);

         bool SetCurrentAnimationByName(const std::string& animationName);

         bool SetCurrentAnimationByIndex(const size_t index);

         std::vector<glm::mat4> GetAnimatedMatrices() const;

         virtual void MakeTransitionToState(BaseState& srcState, BaseState& dstState, const float duration) override;

         virtual void OnTransitionFinished() override;
      };

   }
}


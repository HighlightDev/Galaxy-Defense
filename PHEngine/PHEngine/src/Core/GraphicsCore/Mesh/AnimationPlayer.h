#pragma once

#include "Core/GraphicsCore/Mesh/AnimatedMeshData.h"

namespace Game
{
   class AnimationStateMachineController;
}

namespace Graphics
{
   namespace Mesh
   {

      class AnimationPlayer 
      {
         friend class Game::AnimationStateMachineController;

         std::shared_ptr<AnimatedMeshData> m_animatedMeshData;

         /* this is the main animation time counter*/
         float mSrcAnimationTime;

         /* this time is used when blending of animations is being calculated*/
         float mDstAnimationTime;

         /* this is the main animation name*/
         std::string mSrcAnimationName;

         /* this animation name is used when blending of animations is being occurred*/
         std::string mDstAnimationName;

         std::vector<glm::mat4> mCachedAnimatedMatrices;

      public:

         AnimationPlayer(std::shared_ptr<AnimatedMeshData> animatedData);

         AnimationPlayer() = default;

         void UpdateAnimationTime(const float deltaTime);

         /* call this when just need to update matrices with current animation*/
         void UpdateAnimationMatrices();

         /* call this when need to update matrices during animation transition */
         void UpdateAnimationTransitionMatrices(const float transitionParameter);

         bool SetCurrentAnimationByName(const std::string& animationName);

         bool SetCurrentAnimationByIndex(const size_t index);

         std::vector<glm::mat4> GetAnimatedMatrices() const;

         void SetSrcAnimationName(const std::string& srcAnimationName);

         void SetDstAnimationName(const std::string& dstAnimationName);

      };

   }
}


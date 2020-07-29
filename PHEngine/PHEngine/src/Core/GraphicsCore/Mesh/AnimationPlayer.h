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

         float mTransitionParameter = 0.0f;

         bool bTransitionEnabled = false;

         std::vector<glm::mat4> mCachedAnimatedMatrices;

      public:

         AnimationPlayer(std::shared_ptr<AnimatedMeshData> animatedData);

         AnimationPlayer() = default;

         /* updates matrices with current animation*/
         void UpdateAnimationMatrices();

         bool SetCurrentAnimationByName(const std::string& animationName);

         bool SetCurrentAnimationByIndex(const size_t index);

         std::vector<glm::mat4> GetAnimatedMatrices() const;

         void SetSrcAnimationName(const std::string& srcAnimationName);

         void SetDstAnimationName(const std::string& dstAnimationName);

         void SetSrcAnimationTime(const float srcAnimationTime);

         void SetDstAnimationTime(const float dstAnimationTime);

         void SetTransitionParameter(const bool isTransitionEnabled, const float transitionParam);

      private:

         void UpdateAnimationMatrices_Inner();

      };

   }
}


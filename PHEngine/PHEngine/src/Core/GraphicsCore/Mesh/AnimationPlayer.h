#pragma once

#include <memory>
#include <string.h>
#include <vector>
#include <cstddef>
#include <glm/mat4x4.hpp>

#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"

namespace Game
{
   class AnimationTweenController;
}

namespace Graphics
{
   namespace Mesh
   {

      class AnimationPlayer
      {
         friend class Game::AnimationTweenController;

         std::shared_ptr<AnimatedSkin> m_animatedSkin;

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
         AnimationPlayer(const std::shared_ptr<AnimatedSkin>& animatedSkin);

         AnimationPlayer() = default;

         /* updates matrices with current animation*/
         void UpdateAnimationMatrices();

         const std::vector<glm::mat4> &GetAnimatedMatrices() const;

         bool SetSrcAnimationByIndex(const size_t index);

         bool SetDstAnimationByIndex(const size_t index);

         bool SetSrcAnimationName(const std::string &srcAnimationName);

         bool SetDstAnimationName(const std::string &dstAnimationName);

         void SetSrcAnimationTime(const float srcAnimationTime);

         void SetDstAnimationTime(const float dstAnimationTime);

         void SetTransitionParameter(const bool isTransitionEnabled, const float transitionParam);

         std::string GetSrcAnimationName() const;

         std::string GetDstAnimationName() const;

         float GetSrcAnimationTime() const;

         float GetDstAnimationTime() const;

      private:
         void UpdateAnimationMatrices_Inner();
      };

   }
}

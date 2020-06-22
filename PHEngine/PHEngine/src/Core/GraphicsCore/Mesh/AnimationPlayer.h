#pragma once

#include "AnimatedMeshData.h"

namespace Graphics
{
   namespace Mesh
   {

      class AnimationPlayer
      {
         std::shared_ptr<AnimatedMeshData> m_animatedMeshData;

         /* this is the main animation time counter*/
         float mSrcAnimationTime;

         /* this time is used when blending of animations is being calculated*/
         float mDstAnimationTime;

         bool bBlendingEnabled;

         std::string mSrcAnimationName;

         std::string mDstAnimationName;

      public:

         AnimationPlayer(std::shared_ptr<AnimatedMeshData> animatedData);

         AnimationPlayer() = default;

         void UpdateAnimationTime(const float deltaTime);

         bool SetCurrentAnimationByName(const std::string& animationName);

         bool SetCurrentAnimationByIndex(const size_t index);

         std::vector<glm::mat4> GetAnimatedMatrices() const;
      };

   }
}


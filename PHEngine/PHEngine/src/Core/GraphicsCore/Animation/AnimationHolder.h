#pragma once

#include <vector>
#include <glm/vec4.hpp>

#include "AnimationSequence.h"
#include "BoneTransform.h"
#include "Bone.h"

namespace Graphics
{
   namespace Animation
   {

      class AnimationHolder
      {

         std::shared_ptr<std::vector<AnimationSequence>> m_animationSequences;

         AnimationSequence* m_currentSequence;

         std::vector<BoneTransform> m_cachedBoneTransforms;
         
         std::vector<glm::mat4> m_cachedSkinningMatrices;

         double m_sequenceLoopTime;
         double m_previousFrameTime;

         bool bSequenceDirty = true;

      public:

         AnimationHolder(std::shared_ptr<std::vector<AnimationSequence>> animationSequences);

         ~AnimationHolder();

         void SetAnimationByNameNoBlend(const std::string& animationName);

         void UpdateAnimationLoopTime(float deltaTime);

         AnimationSequence* GetCurrentSequence() const;

         std::vector<BoneTransform>& GetAndUpdateAnimatedPoseTransformsList();

         std::vector<glm::mat4> GetAnimatedOffsetedMatrices(Bone* rootBone);

         std::vector<glm::mat4> GetAnimatedNotOffsetedPoseMatricesList();

         void TransformFromLocalSpaceToBoneSpace(Bone* parentBone, glm::mat4& parentMatrix, std::vector<glm::mat4> srcTransformation, std::vector<glm::mat4>& dstMatrices) const;
      };

   }
}

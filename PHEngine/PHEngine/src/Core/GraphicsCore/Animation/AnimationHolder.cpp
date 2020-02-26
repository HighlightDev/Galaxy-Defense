#include "AnimationHolder.h"

#include <algorithm>

namespace Graphics
{
   namespace Animation
   {

      AnimationHolder::AnimationHolder(std::shared_ptr<std::vector<AnimationSequence>> animationSequences)
         : m_animationSequences(animationSequences)
         , m_currentSequence(nullptr)
         , m_sequenceLoopTime(0.0)
      {
      }

      AnimationHolder::~AnimationHolder()
      {
      }

      void AnimationHolder::SetAnimationByNameNoBlend(const std::string& animationName)
      {
         auto currentSequenceIt = std::find_if(m_animationSequences->begin(), m_animationSequences->end(), [&](const AnimationSequence& animSequence) { return animSequence.GetName() == animationName; });
         m_currentSequence = &(*currentSequenceIt);
         m_sequenceLoopTime = 0.0;
      }

      void AnimationHolder::UpdateAnimationLoopTime(float deltaTime)
      {
         if (m_currentSequence)
         {
            m_sequenceLoopTime += deltaTime;

            double multiplicity = m_currentSequence->GetAnimationDurationInSec();
            if (m_sequenceLoopTime > multiplicity)
            {
               m_sequenceLoopTime -= m_sequenceLoopTime;
            }
            bSequenceDirty = true;
         }
      }

      AnimationSequence* AnimationHolder::GetCurrentSequence() const
      {
         return m_currentSequence;
      }

      std::vector<BoneTransform>& AnimationHolder::GetAndUpdateAnimatedPoseTransformsList()
      {
         // BLEND BONE TRANSFORMS WITHIN CURRENT ANIMATION FRAME AND RETURN THEM

         // Update animation pose only if sequence loop was updated
         if (bSequenceDirty)
         {
            m_cachedBoneTransforms.clear();
            const auto& nextAndPrevFrames = m_currentSequence->GetNextAndPrevFrames(m_sequenceLoopTime);
            size_t bonesCount = nextAndPrevFrames.first.size();

            for (size_t i = 0; i < bonesCount; i++)
            {
               const BoneTransform& nextTransform = nextAndPrevFrames.first[i].second;
               const BoneTransform& prevTransform = nextAndPrevFrames.second[i].second;

               double nextTransformTime = nextAndPrevFrames.first[i].first;
               double prevTransformTime = nextAndPrevFrames.second[i].first;

               m_previousFrameTime = prevTransformTime;

               float blendFactor = (float)((m_sequenceLoopTime - prevTransformTime) / (nextTransformTime - prevTransformTime));

               const BoneTransform& lerpedTransform = BoneTransform::SLerp(prevTransform, nextTransform, blendFactor);
               m_cachedBoneTransforms.emplace_back(std::move(lerpedTransform));
            }
         }

         return m_cachedBoneTransforms;
      }

      std::vector<glm::mat4> AnimationHolder::GetAnimatedNotOffsetedPoseMatricesList()
      {
         std::vector<glm::mat4> resultAnimatedPoseMatrices;
         auto& poseTransforms = GetAndUpdateAnimatedPoseTransformsList();
         for (std::vector<BoneTransform>::iterator it = poseTransforms.begin(); it != poseTransforms.end(); ++it)
         {
            resultAnimatedPoseMatrices.push_back(it->GetToBoneSpaceMatrix());
         }
         return resultAnimatedPoseMatrices;
      }

      std::vector<glm::mat4> AnimationHolder::GetAnimatedOffsetedMatrices(Bone* rootBone)
      {
         if (bSequenceDirty)
         {
            std::vector<glm::mat4> animatePoseMatrices = GetAnimatedNotOffsetedPoseMatricesList();
            std::vector<glm::mat4> offsetMatrices(rootBone->GetChildrenOffsetMatrices());
            std::vector<glm::mat4> animateToBoneSpaceMatrices;

            glm::mat4 matrixOfRootBone(1);
            TransformFromLocalSpaceToBoneSpace(rootBone, matrixOfRootBone, animatePoseMatrices, animateToBoneSpaceMatrices);

            size_t countOfAnimatedBones = animatePoseMatrices.size();
            m_cachedSkinningMatrices.clear();

            for (size_t i = 0; i < countOfAnimatedBones; i++)
            {
               const glm::mat4& animateMatrix = animateToBoneSpaceMatrices[i];
               const glm::mat4& offsetMatrix = offsetMatrices[i];
               m_cachedSkinningMatrices.emplace_back(animateMatrix * offsetMatrix);
            }

            bSequenceDirty = false;
         }

         return m_cachedSkinningMatrices;
      }

      void AnimationHolder::TransformFromLocalSpaceToBoneSpace(Bone* parentBone, glm::mat4& parentMatrix, std::vector<glm::mat4> srcTransformation, std::vector<glm::mat4>& dstMatrices) const
      {
         for (auto& child : parentBone->GetChildren())
         {
            glm::mat4 currentBoneMatrix = parentMatrix * srcTransformation[child->GetId()];
            dstMatrices.push_back(currentBoneMatrix);
            TransformFromLocalSpaceToBoneSpace(child, currentBoneMatrix, srcTransformation, dstMatrices);
         }
      }
   }
}

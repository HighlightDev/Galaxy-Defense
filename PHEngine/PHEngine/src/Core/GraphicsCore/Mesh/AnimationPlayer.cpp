#include "AnimationPlayer.h"

namespace Graphics
{
   namespace Mesh
   {
      AnimationPlayer::AnimationPlayer(std::shared_ptr<AnimatedMeshData> animatedData)
         : m_animatedMeshData(animatedData)
         , mSrcAnimationTime(0.0f)
         , mDstAnimationTime(0.0f)
         , mSrcAnimationName("")
         , mDstAnimationName("")
      {
         assert((m_animatedMeshData));

         // TODO: TEMP SOLUTION
         bool bResult = false;
         if (m_animatedMeshData->AnimationIndices.size() > 5)
         {
            bResult = SetCurrentAnimationByIndex(8);
         }
         else
         {
            bResult = SetCurrentAnimationByIndex(0);
         }

         assert((bResult));
      }

      void AnimationPlayer::UpdateAnimationTime(const float deltaTime)
      {
         mSrcAnimationTime += deltaTime;
      }

      bool AnimationPlayer::SetCurrentAnimationByName(const std::string& animationName)
      {
         bool bResult = false;

         if (bResult = m_animatedMeshData->AnimationMapping.count(animationName))
         {
            mSrcAnimationName = animationName;
         }

         return bResult;
      }

      bool AnimationPlayer::SetCurrentAnimationByIndex(const size_t index)
      {
         assert(m_animatedMeshData->AnimationIndices.size() > index);
         mSrcAnimationName = m_animatedMeshData->AnimationIndices[index];
         return true;
      }

      void AnimationPlayer::SetSrcAnimationName(const std::string& srcAnimationName)
      {
         mSrcAnimationName = srcAnimationName;
      }

      void AnimationPlayer::SetDstAnimationName(const std::string& dstAnimationName)
      {
         mDstAnimationName = dstAnimationName;
      }

      void AnimationPlayer::UpdateAnimationMatrices()
      {
         mCachedAnimatedMatrices = m_animatedMeshData->GetAnimatedMatrices(mSrcAnimationName, mSrcAnimationTime);
      }

      void AnimationPlayer::UpdateAnimationTransitionMatrices(const float transitionParameter)
      {
         auto srcBoneData = std::move(m_animatedMeshData->GetAnimationBoneMapping(mSrcAnimationName, mSrcAnimationTime));
         auto dstBoneData = std::move(m_animatedMeshData->GetAnimationBoneMapping(mDstAnimationName, mDstAnimationTime));
         auto blendedBoneData = std::move(m_animatedMeshData->BlendAnimationBoneMappings(srcBoneData, dstBoneData, transitionParameter));
         mCachedAnimatedMatrices = m_animatedMeshData->GetAnimatedMatricesWithBlendedBoneData(blendedBoneData);
      }

      std::vector<glm::mat4> AnimationPlayer::GetAnimatedMatrices() const
      {
         return mCachedAnimatedMatrices;
      }

   }
}
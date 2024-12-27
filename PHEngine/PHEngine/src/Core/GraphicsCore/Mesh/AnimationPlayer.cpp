#include "AnimationPlayer.h"

namespace Graphics
{
   namespace Mesh
   {
      AnimationPlayer::AnimationPlayer(const std::shared_ptr<AnimatedSkin>& animatedSkin)
         : m_animatedSkin(animatedSkin)
         , mSrcAnimationTime(0.0f)
         , mDstAnimationTime(0.0f)
         , mSrcAnimationName("")
         , mDstAnimationName("")
         , mTransitionParameter(0.0f)
         , bTransitionEnabled(false)
      {
         assert(m_animatedSkin);
         const auto bResult = SetSrcAnimationByIndex(0);
         assert(bResult);
      }

      bool AnimationPlayer::SetDstAnimationByIndex(const size_t index)
      {
         const auto& animationData = m_animatedSkin->GetAnimatedMeshData();
         assert(animationData->AnimationIndices.size() > index);
         mDstAnimationName = animationData->AnimationIndices[index];
         return true;
      }

      bool AnimationPlayer::SetSrcAnimationByIndex(const size_t index)
      {
         const auto& animationData = m_animatedSkin->GetAnimatedMeshData();
         assert(animationData->AnimationIndices.size() > index);
         mSrcAnimationName = animationData->AnimationIndices[index];
         return true;
      }

      bool AnimationPlayer::SetSrcAnimationName(const std::string& srcAnimationName)
      {
         bool bResult = false;

         if (bResult = m_animatedSkin->GetAnimatedMeshData()->AnimationMapping.count(srcAnimationName))
         {
            mSrcAnimationName = srcAnimationName;
         }

         return bResult;
      }

      bool AnimationPlayer::SetDstAnimationName(const std::string& dstAnimationName)
      {
         bool bResult = false;

         if (bResult = m_animatedSkin->GetAnimatedMeshData()->AnimationMapping.count(dstAnimationName))
         {
            mDstAnimationName = dstAnimationName;
         }

         return bResult;
      }

      void AnimationPlayer::SetSrcAnimationTime(const float srcAnimationTime)
      {
         mSrcAnimationTime = srcAnimationTime;
      }

      void AnimationPlayer::SetDstAnimationTime(const float dstAnimationTime)
      {
         mDstAnimationTime = dstAnimationTime;
      }

      std::string AnimationPlayer::GetSrcAnimationName() const
      {
         return mSrcAnimationName;
      }

      std::string AnimationPlayer::GetDstAnimationName() const
      {
         return mDstAnimationName;
      }

      float AnimationPlayer::GetSrcAnimationTime() const
      {
         return mSrcAnimationTime;
      }

      float AnimationPlayer::GetDstAnimationTime() const
      {
         return mDstAnimationTime;
      }

      void AnimationPlayer::SetTransitionParameter(const bool isTransitionEnabled, const float transitionParam)
      {
         bTransitionEnabled = isTransitionEnabled;
         mTransitionParameter = transitionParam;
      }

      void AnimationPlayer::UpdateAnimationMatrices()
      {
         UpdateAnimationMatrices_Inner();
      }

      void AnimationPlayer::UpdateAnimationMatrices_Inner()
      {
         const auto& animationData = m_animatedSkin->GetAnimatedMeshData();
         if (bTransitionEnabled)
         {
            const auto& blendedBoneData = animationData->GetBoneMappingForBlendedAnimation(mSrcAnimationName, mDstAnimationName,
               mSrcAnimationTime, mDstAnimationTime, mTransitionParameter);
            mCachedAnimatedMatrices = animationData->GetAnimatedMatricesWithBlendedBoneData(blendedBoneData);
         }
         else
         {
            mCachedAnimatedMatrices = animationData->GetAnimatedMatrices(mSrcAnimationName, mSrcAnimationTime);
         }
      }

      const std::vector<glm::mat4>& AnimationPlayer::GetAnimatedMatrices() const
      {
         return mCachedAnimatedMatrices;
      }

   }
}
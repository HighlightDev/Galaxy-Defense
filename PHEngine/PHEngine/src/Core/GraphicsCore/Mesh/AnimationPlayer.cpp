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
         , mTransitionParameter (0.0f)
      ,bTransitionEnabled( false)
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

      void AnimationPlayer::SetSrcAnimationTime(const float srcAnimationTime)
      {
         mSrcAnimationTime = srcAnimationTime;
      }

      void AnimationPlayer::SetDstAnimationTime(const float dstAnimationTime)
      {
         mDstAnimationTime = dstAnimationTime;
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
         if (bTransitionEnabled)
         {
            auto srcBoneData = std::move(m_animatedMeshData->GetAnimationBoneMapping(mSrcAnimationName, mSrcAnimationTime));
            auto dstBoneData = std::move(m_animatedMeshData->GetAnimationBoneMapping(mDstAnimationName, mDstAnimationTime));
            auto blendedBoneData = std::move(m_animatedMeshData->BlendAnimationBoneMappings(srcBoneData, dstBoneData, mTransitionParameter));
            mCachedAnimatedMatrices = m_animatedMeshData->GetAnimatedMatricesWithBlendedBoneData(blendedBoneData);
         }
         else
         {
            mCachedAnimatedMatrices = m_animatedMeshData->GetAnimatedMatrices(mSrcAnimationName, mSrcAnimationTime);
         }
      }

      std::vector<glm::mat4> AnimationPlayer::GetAnimatedMatrices() const
      {
         return mCachedAnimatedMatrices;
      }

   }
}
#include "AnimationPlayer.h"

// todo: remove this part later
#include <chrono>
#include <iostream>

namespace Graphics
{
   namespace Mesh
   {
      using Clock_t = std::chrono::high_resolution_clock;

      AnimationPlayer::AnimationPlayer(std::shared_ptr<AnimatedMeshData> animatedData)
         : m_animatedMeshData(animatedData)
         , mSrcAnimationTime(0.0f)
         , mDstAnimationTime(0.0f)
         , mSrcAnimationName("")
         , mDstAnimationName("")
         , mTransitionParameter(0.0f)
         , bTransitionEnabled(false)
      {
         assert((m_animatedMeshData));

         // TODO: TEMP SOLUTION
         bool bResult = false;
         if (m_animatedMeshData->AnimationIndices.size() > 5)
         {
            bResult = SetSrcAnimationByIndex(8);
         }
         else
         {
            bResult = SetSrcAnimationByIndex(0);
         }

         assert((bResult));
      }

      bool AnimationPlayer::SetDstAnimationByIndex(const size_t index)
      {
         assert(m_animatedMeshData->AnimationIndices.size() > index);
         mDstAnimationName = m_animatedMeshData->AnimationIndices[index];
         return true;
      }

      bool AnimationPlayer::SetSrcAnimationByIndex(const size_t index)
      {
         assert(m_animatedMeshData->AnimationIndices.size() > index);
         mSrcAnimationName = m_animatedMeshData->AnimationIndices[index];
         return true;
      }

      bool AnimationPlayer::SetSrcAnimationName(const std::string& srcAnimationName)
      {
         bool bResult = false;

         if (bResult = m_animatedMeshData->AnimationMapping.count(srcAnimationName))
         {
            mSrcAnimationName = srcAnimationName;
         }

         return bResult;
      }

      bool AnimationPlayer::SetDstAnimationName(const std::string& dstAnimationName)
      {
         bool bResult = false;

         if (bResult = m_animatedMeshData->AnimationMapping.count(dstAnimationName))
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

      // todo: remove this part later
      double longestTime = 0.0;

      void AnimationPlayer::UpdateAnimationMatrices_Inner()
      {
         Clock_t::time_point time1;

         time1 = Clock_t::now();
         if (bTransitionEnabled)
         {
            auto blendedBoneData = m_animatedMeshData->GetBoneMappingForBlendedAnimation(mSrcAnimationName, mDstAnimationName,
               mSrcAnimationTime, mDstAnimationTime, mTransitionParameter);
            mCachedAnimatedMatrices = m_animatedMeshData->GetAnimatedMatricesWithBlendedBoneData(blendedBoneData);
         }
         else
         {
            mCachedAnimatedMatrices = m_animatedMeshData->GetAnimatedMatrices(mSrcAnimationName, mSrcAnimationTime);
         }

         Clock_t::duration deltaTime = Clock_t::now() - time1;

         const double invFromNanoToSec = 0.000000001;

         auto time = static_cast<double>(deltaTime.count()) * invFromNanoToSec;
         if (time > longestTime)
            longestTime = time;

         //std::cout << "Time For Blending : " << longestTime << std::endl;
      }

      const std::vector<glm::mat4>& AnimationPlayer::GetAnimatedMatrices() const
      {
         return mCachedAnimatedMatrices;
      }

   }
}
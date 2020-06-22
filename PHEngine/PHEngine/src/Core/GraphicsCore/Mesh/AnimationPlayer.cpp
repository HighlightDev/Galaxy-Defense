#include "AnimationPlayer.h"

namespace Graphics
{
   namespace Mesh
   {
      AnimationPlayer::AnimationPlayer(std::shared_ptr<AnimatedMeshData> animatedData)
         : m_animatedMeshData(animatedData)
         , mSrcAnimationTime(0.0f)
         , mDstAnimationTime(0.0f)
         , bBlendingEnabled(false)
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

         if (bBlendingEnabled)
            mDstAnimationTime += deltaTime;
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

      std::vector<glm::mat4> AnimationPlayer::GetAnimatedMatrices() const
      {
         std::vector<glm::mat4> result;
         if (bBlendingEnabled)
         {
            // todo: 
            // m_animatedMeshData->GetAnimationBoneData(mSrcAnimationName, mSrcAnimationTime);
            // m_animatedMeshData->GetAnimationBoneData(mDstAnimationName, mDstAnimationTime);
            // blend and get result
            // result = ...
         }
         else
         {
            result = m_animatedMeshData->GetAnimatedMatrices(mSrcAnimationName, mSrcAnimationTime);
         }

         return result;
      }

   }
}
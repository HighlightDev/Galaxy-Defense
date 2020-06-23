#include "AnimationPlayer.h"

namespace Graphics
{
   namespace Mesh
   {
      AnimationPlayer::AnimationPlayer(std::shared_ptr<AnimatedMeshData> animatedData)
         : IStateMachineController()
         , m_animatedMeshData(animatedData)
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

         UpdateTransitionTime(deltaTime);

         if (bTransitionEnabled)
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

      void AnimationPlayer::MakeTransitionToState(BaseState& srcState, BaseState& dstState, const float transitionDuration)
      {
         IStateMachineController::MakeTransitionToState(srcState, dstState, transitionDuration);

         mDstAnimationName = ""; // dstState;
      }

      void AnimationPlayer::OnTransitionFinished()
      {
         mSrcAnimationName = mDstAnimationName;
         mSrcAnimationTime = mDstAnimationTime;
         mDstAnimationTime = 0.0f;
         mDstAnimationName = "NoAnimation";
      }

      std::vector<glm::mat4> AnimationPlayer::GetAnimatedMatrices() const
      {
         std::vector<glm::mat4> result;
         if (bTransitionEnabled)
         {
            auto srcBoneData = std::move(m_animatedMeshData->GetAnimationBoneMapping(mSrcAnimationName, mSrcAnimationTime));
            auto dstBoneData = std::move(m_animatedMeshData->GetAnimationBoneMapping(mDstAnimationName, mDstAnimationTime));
            auto blendedBoneData = std::move(m_animatedMeshData->BlendAnimationBoneMappings(srcBoneData, dstBoneData, transitionParameter));
            result = m_animatedMeshData->GetAnimatedMatricesWithBlendedBoneData(blendedBoneData);
         }
         else
         {
            result = m_animatedMeshData->GetAnimatedMatrices(mSrcAnimationName, mSrcAnimationTime);
         }

         return result;
      }

   }
}
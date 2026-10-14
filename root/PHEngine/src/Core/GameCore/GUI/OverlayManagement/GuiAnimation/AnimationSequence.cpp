#include "AnimationSequence.h"

namespace EngineCore::GUI {
void AnimationSequence::SetAnimationDataInSequence(const std::vector<AnimationData>& animationDataList)
{
    mAnimationDataInSequence = animationDataList;
}

void AnimationSequence::SetCurrentAnimationDataIndex(const int32_t animationDataIndex)
{
    mCurrentAnimationDataIndex = animationDataIndex;
}

const std::vector<AnimationData>& AnimationSequence::GetAnimationDataInSequence() const
{
    return mAnimationDataInSequence;
}

int32_t AnimationSequence::GetCurrentAnimationDataIndex() const
{
    return mCurrentAnimationDataIndex;
}

void AnimationSequence::SetPassedAnimationTime(const float animationTime)
{
    mAnimationTimePassed = animationTime;
}

void AnimationSequence::AddDeltaTimeToAnimationTime(const float deltaTimeSec)
{
    mAnimationTimePassed += deltaTimeSec;
}

float AnimationSequence::GetPassedAnimationTime() const
{
    return mAnimationTimePassed;
}

bool AnimationSequence::TryToMoveToNextAnimation()
{
    mAnimationTimePassed = 0.0f;
    if (mAnimationDataInSequence.size() > ++mCurrentAnimationDataIndex) {
        return true;
    }
    // Animation sequence is finished
    mCurrentAnimationDataIndex = -1;
    return false;
}
} // namespace EngineCore::GUI

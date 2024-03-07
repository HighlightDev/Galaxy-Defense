#pragma once

#include "AnimationData.h"

#include <vector>

namespace EngineCore::GUI
{
    class AnimationSequence
    {
        std::vector<AnimationData> mAnimationDataInSequence;

        int32_t mCurrentAnimationDataIndex{-1};

        float mAnimationTimePassed{0.0f};

    public:
        void SetAnimationDataInSequence(const std::vector<AnimationData> &animationDataList);

        void SetCurrentAnimationDataIndex(const int32_t animationDataIndex);

        void SetPassedAnimationTime(const float animationTime);

        void AddDeltaTimeToAnimationTime(const float deltaTime);

        const std::vector<AnimationData> &GetAnimationDataInSequence() const;

        int32_t GetCurrentAnimationDataIndex() const;

        float GetPassedAnimationTime() const;

        bool TryToMoveToNextAnimation();
    };
}

#include "AnimationData.h"

namespace EngineCore
{
    namespace GUI
    {
        AnimationData::AnimationData(const eAnimationInterpolationFunctionType animationFunctionType,
                                     const float animationDuration,
                                     const std::string &propertyName,
                                     const std::any &valueFrom,
                                     const std::any &valueTo)
            : mFunctionType(animationFunctionType),
              mAnimationDuration(animationDuration),
              mPropertyName(propertyName),
              mSrcValue(valueFrom),
              mDstValue(valueTo)
        {
        }

        eAnimationInterpolationFunctionType AnimationData::GetAnimationFunctionType() const
        {
            return mFunctionType;
        }

        float AnimationData::GetAnimationDuration() const
        {
            return mAnimationDuration;
        }

        std::string AnimationData::GetPropertyName() const
        {
            return mPropertyName;
        }

        const std::any &AnimationData::GetSrcValue() const
        {
            return mSrcValue;
        }

        const std::any &AnimationData::GetDstValue() const
        {
            return mDstValue;
        }
    }
}

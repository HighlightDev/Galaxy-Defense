#pragma once

#include "AnimationInterpolationFunctionType.h"

#include <any>
#include <memory>
#include <string>

namespace EngineCore {
namespace GUI {
class AnimationData {
    eAnimationInterpolationFunctionType mFunctionType{eAnimationInterpolationFunctionType::LINEAR};

    float mAnimationDuration;

    std::string mPropertyName;

    std::any mSrcValue;

    std::any mDstValue;

public:
    explicit AnimationData(
        const eAnimationInterpolationFunctionType animationFunctionType,
        const float animationDuration,
        const std::string& propertyName,
        const std::any& valueFrom,
        const std::any& valueTo);

    eAnimationInterpolationFunctionType GetAnimationFunctionType() const;

    float GetAnimationDuration() const;

    std::string GetPropertyName() const;

    const std::any& GetSrcValue() const;
    const std::any& GetDstValue() const;
};
} // namespace GUI
} // namespace EngineCore

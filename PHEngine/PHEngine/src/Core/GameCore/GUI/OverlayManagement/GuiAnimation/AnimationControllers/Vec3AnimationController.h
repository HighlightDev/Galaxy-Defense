#pragma once

#include "IAnimationController.h"

namespace EngineCore {
namespace GUI {
class AnimationData;

class Vec3AnimationController : public IAnimationController {
public:
    void ProcessAnimation(
        const float animationTimePassed,
        const ::EngineCore::GUI::AnimationData& data,
        const std::shared_ptr<::EngineCore::GUI::IAnimatable>& animatable) override;

    void ForceFinishAnimation(
        const ::EngineCore::GUI::AnimationData& data, const std::shared_ptr<::EngineCore::GUI::IAnimatable>& animatable) override;

    void InitWithSrcValues(
        const ::EngineCore::GUI::AnimationData& data, const std::shared_ptr<::EngineCore::GUI::IAnimatable>& animatable) override;

    eEnginePropertyType GetPropertyType() const override;
};
} // namespace GUI

} // namespace EngineCore

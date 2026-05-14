#pragma once
#include "ITweenController.h"
#include "StateProperty.h"

namespace EngineCore {

class Vec3QuadraticBezierTweenController : public ITweenController {
    using Base = ITweenController;
    using TweenStateProperty_t = StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier>;

public:
    Vec3QuadraticBezierTweenController();
    virtual ~Vec3QuadraticBezierTweenController();

    void OnTransitionStarted(
        const std::shared_ptr<BaseStateProperty>& srcState,
        const std::shared_ptr<BaseStateProperty>& dstState,
        const float duration) override;

    void OnTransitionFinished() override;

    void OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter) override;

    void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty) override;

private:
    std::shared_ptr<Vec3QuadraticBezierPropertyBinding> GetVec3QuadraticBezierPropertyBindingSP() const;
};

} // namespace EngineCore

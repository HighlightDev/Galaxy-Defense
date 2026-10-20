#pragma once
#include "ITweenController.h"
#include "StateProperty.h"

namespace EngineCore {

class EulerAnglesRotationTweenController : public ITweenController {
    using Base = ITweenController;
    using TweenStateProperty_t = StateProperty<eEnginePropertyBindingType::EulerAnglesRotation>;

public:
    EulerAnglesRotationTweenController();
    virtual ~EulerAnglesRotationTweenController();

    void OnTransitionStarted(
        const std::shared_ptr<BaseStateProperty>& srcState,
        const std::shared_ptr<BaseStateProperty>& dstState,
        const float duration,
        const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty) override;

    void OnTransitionFinished() override;

    void OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter) override;

    void InitWithPropsInstant(
        const std::shared_ptr<BaseStateProperty>& dstStateProperty,
        const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty) override;
};

} // namespace EngineCore

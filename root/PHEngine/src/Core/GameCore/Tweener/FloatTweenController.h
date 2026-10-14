#pragma once
#include "ITweenController.h"
#include "StateProperty.h"

namespace EngineCore {

class FloatTweenController : public ITweenController {
    using Base = ITweenController;
    using TweenStateProperty_t = StateProperty<eEnginePropertyBindingType::FloatScalar>;

public:
    FloatTweenController();
    virtual ~FloatTweenController();

    void OnTransitionStarted(
        const std::shared_ptr<BaseStateProperty>& srcState,
        const std::shared_ptr<BaseStateProperty>& dstState,
        const float duration) override;

    void OnTransitionFinished() override;

    void OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter) override;

    void InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty) override;

private:
    std::shared_ptr<FloatPropertyBinding> GetFloatPropertyBindingSP() const;
};

} // namespace EngineCore

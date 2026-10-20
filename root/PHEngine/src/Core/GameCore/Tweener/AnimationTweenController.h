#pragma once

#include "Core/GraphicsCore/Mesh/AnimationPlayer.h"
#include "ITweenController.h"
#include "StateProperty.h"

using namespace Graphics::Mesh;

namespace EngineCore {
struct BaseStateProperty;

class AnimationTweenController : public ITweenController {
    using Base = ITweenController;
    using TweenStateProperty_t = StateProperty<eEnginePropertyBindingType::Animation>;

public:
    AnimationTweenController();

    void OnTransitionStarted(
        const std::shared_ptr<BaseStateProperty>&,
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

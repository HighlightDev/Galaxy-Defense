#include "AnimationTweenController.h"

#include "StateProperty.h"

namespace EngineCore {
AnimationTweenController::AnimationTweenController()
    : ITweenController()
{
}

void AnimationTweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto animProperty = GetEngineObjectPropertySp<AnimationBlendState>()) {
        AnimationBlendState animState = animProperty->GetValue();
        animState.DstTime += deltaTimeSec;
        animState.bTransitionEnabled = true;
        animState.TransitionValue = transitionParameter;
        animProperty->SetValue(animState);
    }
}

void AnimationTweenController::InitWithPropsInstant(
    const std::shared_ptr<BaseStateProperty>& dstStateProperty,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::InitWithPropsInstant(dstStateProperty, engineObjectProperty);
    if (auto animProperty = GetEngineObjectPropertySp<AnimationBlendState>()) {
        auto dstAnimationProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        AnimationBlendState animState;
        animState.SrcName = dstAnimationProperty->AnimationName;
        animState.SrcTime = 0.0f;
        animState.DstTime = 0.0f;
        animState.DstName = "NoAnimation";
        animState.bTransitionEnabled = false;
        animState.TransitionValue = 0.0f;
        animProperty->SetValue(animState);
    }
}

void AnimationTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration, engineObjectProperty);

    auto srcAnimationProperty = std::static_pointer_cast<TweenStateProperty_t>(srcProperty);
    auto dstAnimationProperty = std::static_pointer_cast<TweenStateProperty_t>(dstProperty);

    if (auto animProperty = GetEngineObjectPropertySp<AnimationBlendState>()) {
        AnimationBlendState animState = animProperty->GetValue();
        animState.SrcName = srcAnimationProperty->AnimationName;
        animState.DstName = dstAnimationProperty->AnimationName;
        animState.DstTime = 0.0f;
        animState.bTransitionEnabled = true;
        animState.TransitionValue = 0.0f;
        animProperty->SetValue(animState);
    }
}

void AnimationTweenController::OnTransitionFinished()
{
    if (auto animProperty = GetEngineObjectPropertySp<AnimationBlendState>()) {
        AnimationBlendState animState = animProperty->GetValue();
        animState.SrcName = animState.DstName;
        animState.SrcTime = animState.DstTime;
        animState.DstTime = 0.0f;
        animState.DstName = "NoAnimation";
        animState.bTransitionEnabled = false;
        animState.TransitionValue = 0.0f;
        animProperty->SetValue(animState);
    }
}
} // namespace EngineCore

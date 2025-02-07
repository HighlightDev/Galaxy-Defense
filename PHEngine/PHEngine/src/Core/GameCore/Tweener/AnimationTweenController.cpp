#include "AnimationTweenController.h"

#include "StateProperty.h"

namespace EngineCore {
AnimationTweenController::AnimationTweenController()
    : ITweenController()
{
}

std::shared_ptr<AnimationPropertyBinding> AnimationTweenController::GetAnimationPropertyBindingSP() const
{
    std::shared_ptr<AnimationPropertyBinding> result(nullptr);

    if (auto baseSp = mPropertyBinding.lock()) {
        result = std::static_pointer_cast<AnimationPropertyBinding>(baseSp);
    }

    return result;
}

void AnimationTweenController::OnTransitionUpdate(const float deltaTime, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTime, transitionParameter);

    if (auto animBinding = GetAnimationPropertyBindingSP()) {
        animBinding->SetDstTime(animBinding->GetDstTime() + deltaTime);
        animBinding->SetIsTransitionEnabled(true);
        animBinding->SetTransitionValue(transitionParameter);
    }
}

void AnimationTweenController::InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty)
{
    mPropertyBinding = dstStateProperty->Binding;
    if (auto animBinding = GetAnimationPropertyBindingSP()) {
        auto dstAnimationProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        animBinding->SetSrcName(dstAnimationProperty->AnimationName);
        animBinding->SetSrcTime(0.0f);
        animBinding->SetDstTime(0.0f);
        animBinding->SetDstName("NoAnimation");
        animBinding->SetIsTransitionEnabled(false);
        animBinding->SetTransitionValue(0.0f);
    }
}

void AnimationTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

    auto srcAnimationProperty = std::static_pointer_cast<TweenStateProperty_t>(srcProperty);
    auto dstAnimationProperty = std::static_pointer_cast<TweenStateProperty_t>(dstProperty);

    if (auto animBinding = GetAnimationPropertyBindingSP()) {
        animBinding->SetSrcName(srcAnimationProperty->AnimationName);
        animBinding->SetDstName(dstAnimationProperty->AnimationName);
        animBinding->SetDstTime(0.0f);
        animBinding->SetIsTransitionEnabled(true);
        animBinding->SetTransitionValue(0.0f);
    }
}

void AnimationTweenController::OnTransitionFinished()
{
    if (auto animBinding = GetAnimationPropertyBindingSP()) {
        animBinding->SetSrcName(animBinding->GetDstName());
        animBinding->SetSrcTime(animBinding->GetDstTime());
        animBinding->SetDstTime(0.0f);
        animBinding->SetDstName("NoAnimation");
        animBinding->SetIsTransitionEnabled(false);
        animBinding->SetTransitionValue(0.0f);
    }
}
} // namespace EngineCore
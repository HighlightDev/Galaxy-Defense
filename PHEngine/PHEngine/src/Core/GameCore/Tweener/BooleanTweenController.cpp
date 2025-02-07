#include "BooleanTweenController.h"

#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {

BooleanTweenController::BooleanTweenController()
    : ITweenController()
{
}

BooleanTweenController::~BooleanTweenController()
{
}

std::shared_ptr<BooleanPropertyBinding> BooleanTweenController::GetBooleanPropertyBindingSP() const
{
    std::shared_ptr<BooleanPropertyBinding> result(nullptr);

    if (auto baseSp = mPropertyBinding.lock()) {
        result = std::static_pointer_cast<BooleanPropertyBinding>(baseSp);
        assert(result);
    }

    return result;
}

void BooleanTweenController::OnTransitionUpdate(const float deltaTime, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTime, transitionParameter);

    if (auto booleanBinding = GetBooleanPropertyBindingSP()) {
        auto srcBoolProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        booleanBinding->SetValue(srcBoolProperty->Value);
    }
}

void BooleanTweenController::InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty)
{
    mPropertyBinding = dstStateProperty->Binding;
    if (auto booleanBinding = GetBooleanPropertyBindingSP()) {
        auto dstBoolProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);
        booleanBinding->SetValue(dstBoolProperty->Value);
    }
}

void BooleanTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

    if (auto booleanBinding = GetBooleanPropertyBindingSP()) {
        auto srcBoolProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        booleanBinding->SetValue(srcBoolProperty->Value);
    }
}

void BooleanTweenController::OnTransitionFinished()
{
    if (auto booleanBinding = GetBooleanPropertyBindingSP()) {
        auto dstBoolProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);
        booleanBinding->SetValue(dstBoolProperty->Value);
    }
}

} // namespace EngineCore

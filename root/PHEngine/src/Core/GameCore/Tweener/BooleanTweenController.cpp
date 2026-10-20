#include "BooleanTweenController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {

BooleanTweenController::BooleanTweenController()
    : ITweenController()
{
}

BooleanTweenController::~BooleanTweenController()
{
}

void BooleanTweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto booleanProperty = GetEngineObjectPropertySp<bool>()) {
        auto srcBoolProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        booleanProperty->SetValue(srcBoolProperty->Value);
    }
}

void BooleanTweenController::InitWithPropsInstant(
    const std::shared_ptr<BaseStateProperty>& dstStateProperty,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::InitWithPropsInstant(dstStateProperty, engineObjectProperty);
    if (auto booleanProperty = GetEngineObjectPropertySp<bool>()) {
        auto dstBoolProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);
        booleanProperty->SetValue(dstBoolProperty->Value);
    }
}

void BooleanTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration, engineObjectProperty);

    if (auto booleanProperty = GetEngineObjectPropertySp<bool>()) {
        auto srcBoolProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        booleanProperty->SetValue(srcBoolProperty->Value);
    }
}

void BooleanTweenController::OnTransitionFinished()
{
    if (auto booleanProperty = GetEngineObjectPropertySp<bool>()) {
        auto dstBoolProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);
        booleanProperty->SetValue(dstBoolProperty->Value);
    }
}

} // namespace EngineCore

#include "FloatTweenController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {

FloatTweenController::FloatTweenController()
    : ITweenController()
{
}

FloatTweenController::~FloatTweenController()
{
}

void FloatTweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto floatProperty = GetEngineObjectPropertySp<float>()) {
        auto srcFloatProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        auto dstFloatProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        floatProperty->SetValue(
            EngineMath::LerpNormalizedFloat(srcFloatProperty->Value, dstFloatProperty->Value, transitionParameter));
    }
}

void FloatTweenController::InitWithPropsInstant(
    const std::shared_ptr<BaseStateProperty>& dstStateProperty,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::InitWithPropsInstant(dstStateProperty, engineObjectProperty);
    if (auto floatProperty = GetEngineObjectPropertySp<float>()) {
        auto dstFloatProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        floatProperty->SetValue(dstFloatProperty->Value);
    }
}

void FloatTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration, engineObjectProperty);

    if (auto floatProperty = GetEngineObjectPropertySp<float>()) {
        auto srcFloatProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        floatProperty->SetValue(srcFloatProperty->Value);
    }
}

void FloatTweenController::OnTransitionFinished()
{
    if (auto floatProperty = GetEngineObjectPropertySp<float>()) {
        auto dstFloatProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        floatProperty->SetValue(dstFloatProperty->Value);
    }
}

} // namespace EngineCore

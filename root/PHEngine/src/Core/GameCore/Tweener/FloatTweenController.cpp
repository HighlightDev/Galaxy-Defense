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

std::shared_ptr<FloatPropertyBinding> FloatTweenController::GetFloatPropertyBindingSP() const
{
    std::shared_ptr<FloatPropertyBinding> result(nullptr);

    if (auto baseSp = mPropertyBinding.lock()) {
        result = std::static_pointer_cast<FloatPropertyBinding>(baseSp);
        ext_assert(result, "FloatTweenController::GetFloatPropertyBindingSP: Failed to cast to FloatPropertyBinding");
    }

    return result;
}

void FloatTweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto floatBinding = GetFloatPropertyBindingSP()) {
        auto srcFloatProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        auto dstFloatProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        floatBinding->SetValue(
            EngineMath::LerpNormalizedFloat(srcFloatProperty->Value, dstFloatProperty->Value, transitionParameter));
    }
}

void FloatTweenController::InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty)
{
    mPropertyBinding = dstStateProperty->Binding;
    if (auto floatBinding = GetFloatPropertyBindingSP()) {
        auto dstFloatProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        floatBinding->SetValue(dstFloatProperty->Value);
    }
}

void FloatTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

    if (auto floatBinding = GetFloatPropertyBindingSP()) {
        auto srcFloatProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        floatBinding->SetValue(srcFloatProperty->Value);
    }
}

void FloatTweenController::OnTransitionFinished()
{
    if (auto floatBinding = GetFloatPropertyBindingSP()) {
        auto dstFloatProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        floatBinding->SetValue(dstFloatProperty->Value);
    }
}

} // namespace EngineCore

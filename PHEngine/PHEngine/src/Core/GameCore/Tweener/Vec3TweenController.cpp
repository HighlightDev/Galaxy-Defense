#include "Vec3TweenController.h"

#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {

Vec3TweenController::Vec3TweenController()
    : ITweenController()
{
}

Vec3TweenController::~Vec3TweenController()
{
}

std::shared_ptr<Vec3PropertyBinding> Vec3TweenController::GetVec3PropertyBindingSP() const
{
    std::shared_ptr<Vec3PropertyBinding> result(nullptr);

    if (auto baseSp = mPropertyBinding.lock()) {
        result = std::static_pointer_cast<Vec3PropertyBinding>(baseSp);
        assert(result);
    }

    return result;
}

void Vec3TweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto vec3Binding = GetVec3PropertyBindingSP()) {
        auto srcVec3Property = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        auto dstVec3Property
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        const auto& srcVec = srcVec3Property->Value;
        const auto& dstVec = dstVec3Property->Value;

        vec3Binding->SetValue(EngineMath::LerpVec3(transitionParameter, 0.0f, 1.0f, srcVec, dstVec));
    }
}

void Vec3TweenController::InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty)
{
    mPropertyBinding = dstStateProperty->Binding;
    if (auto vec3Binding = GetVec3PropertyBindingSP()) {
        auto dstVec3Property = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        vec3Binding->SetValue(dstVec3Property->Value);
    }
}

void Vec3TweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

    if (auto vec3Binding = GetVec3PropertyBindingSP()) {
        auto srcVec3Property = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        vec3Binding->SetValue(srcVec3Property->Value);
    }
}

void Vec3TweenController::OnTransitionFinished()
{
    if (auto vec3Binding = GetVec3PropertyBindingSP()) {
        auto dstVec3Property
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        vec3Binding->SetValue(dstVec3Property->Value);
    }
}

} // namespace EngineCore

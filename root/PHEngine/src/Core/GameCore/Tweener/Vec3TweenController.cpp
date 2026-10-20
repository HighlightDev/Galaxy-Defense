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

void Vec3TweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto vec3Property = GetEngineObjectPropertySp<glm::vec3>()) {
        auto srcVec3Property = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        auto dstVec3Property
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        const auto& srcVec = srcVec3Property->Value;
        const auto& dstVec = dstVec3Property->Value;

        vec3Property->SetValue(EngineMath::LerpVec3(transitionParameter, 0.0f, 1.0f, srcVec, dstVec));
    }
}

void Vec3TweenController::InitWithPropsInstant(
    const std::shared_ptr<BaseStateProperty>& dstStateProperty,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::InitWithPropsInstant(dstStateProperty, engineObjectProperty);
    if (auto vec3Property = GetEngineObjectPropertySp<glm::vec3>()) {
        auto dstVec3Property = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        vec3Property->SetValue(dstVec3Property->Value);
    }
}

void Vec3TweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration, engineObjectProperty);

    if (auto vec3Property = GetEngineObjectPropertySp<glm::vec3>()) {
        auto srcVec3Property = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        vec3Property->SetValue(srcVec3Property->Value);
    }
}

void Vec3TweenController::OnTransitionFinished()
{
    if (auto vec3Property = GetEngineObjectPropertySp<glm::vec3>()) {
        auto dstVec3Property
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        vec3Property->SetValue(dstVec3Property->Value);
    }
}

} // namespace EngineCore

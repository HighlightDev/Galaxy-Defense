#include "Vec3QuadraticBezierTweenController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/ext/quaternion_float.hpp>

namespace EngineCore {

Vec3QuadraticBezierTweenController::Vec3QuadraticBezierTweenController()
    : ITweenController()
{
}

Vec3QuadraticBezierTweenController::~Vec3QuadraticBezierTweenController()
{
}

void Vec3QuadraticBezierTweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto vec3Property = GetEngineObjectPropertySp<glm::vec3>()) {
        auto srcBezierProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        auto dstBezierProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        const glm::vec3 srcValue = srcBezierProperty->Value;
        const glm::vec3 srcControlPoint = srcBezierProperty->ControlPoint;
        const glm::vec3 dstValue = dstBezierProperty->Value;
        const glm::vec3 dstControlPoint = dstBezierProperty->ControlPoint;
        const glm::vec3 finalControlPoint
            = (srcControlPoint + dstControlPoint) * 0.5f; // simple way to calculate control point for bezier curve
        vec3Property->SetValue(EngineMath::QuadraticBezier(srcValue, finalControlPoint, dstValue, transitionParameter));
    }
}

void Vec3QuadraticBezierTweenController::InitWithPropsInstant(
    const std::shared_ptr<BaseStateProperty>& dstStateProperty,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::InitWithPropsInstant(dstStateProperty, engineObjectProperty);
    if (auto vec3Property = GetEngineObjectPropertySp<glm::vec3>()) {
        auto dstBezierProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        vec3Property->SetValue(dstBezierProperty->Value);
    }
}

void Vec3QuadraticBezierTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration, engineObjectProperty);

    if (auto vec3Property = GetEngineObjectPropertySp<glm::vec3>()) {
        auto srcBezierProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        vec3Property->SetValue(srcBezierProperty->Value);
    }
}

void Vec3QuadraticBezierTweenController::OnTransitionFinished()
{
    if (auto vec3Property = GetEngineObjectPropertySp<glm::vec3>()) {
        auto dstBezierProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        vec3Property->SetValue(dstBezierProperty->Value);
    }
}

} // namespace EngineCore

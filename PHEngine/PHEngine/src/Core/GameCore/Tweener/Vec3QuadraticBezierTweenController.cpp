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

std::shared_ptr<Vec3QuadraticBezierPropertyBinding>
Vec3QuadraticBezierTweenController::GetVec3QuadraticBezierPropertyBindingSP() const
{
    std::shared_ptr<Vec3QuadraticBezierPropertyBinding> result(nullptr);

    if (auto baseSp = mPropertyBinding.lock()) {
        result = std::static_pointer_cast<Vec3QuadraticBezierPropertyBinding>(baseSp);
        ext_assert(
            result,
            "Vec3QuadraticBezierTweenController::GetVec3QuadraticBezierPropertyBindingSP: Failed to cast to "
            "Vec3QuadraticBezierPropertyBinding");
    }

    return result;
}

void Vec3QuadraticBezierTweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto bezierBinding = GetVec3QuadraticBezierPropertyBindingSP()) {
        auto srcBezierProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        auto dstBezierProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        const glm::vec3 srcValue = srcBezierProperty->Value;
        const glm::vec3 srcControlPoint = srcBezierProperty->ControlPoint;
        const glm::vec3 dstValue = dstBezierProperty->Value;
        const glm::vec3 dstControlPoint = dstBezierProperty->ControlPoint;
        const glm::vec3 finalControlPoint
            = (srcControlPoint + dstControlPoint) * 0.5f; // simple way to calculate control point for bezier curve
        bezierBinding->SetValue(
            EngineMath::QuadraticBezier(srcValue, finalControlPoint, dstValue, transitionParameter), finalControlPoint);
    }
}

void Vec3QuadraticBezierTweenController::InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty)
{
    mPropertyBinding = dstStateProperty->Binding;
    if (auto bezierBinding = GetVec3QuadraticBezierPropertyBindingSP()) {
        auto dstBezierProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        bezierBinding->SetValue(dstBezierProperty->Value, dstBezierProperty->ControlPoint);
    }
}

void Vec3QuadraticBezierTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration);

    if (auto bezierBinding = GetVec3QuadraticBezierPropertyBindingSP()) {
        auto srcBezierProperty = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        bezierBinding->SetValue(srcBezierProperty->Value, srcBezierProperty->ControlPoint);
    }
}

void Vec3QuadraticBezierTweenController::OnTransitionFinished()
{
    if (auto bezierBinding = GetVec3QuadraticBezierPropertyBindingSP()) {
        auto dstBezierProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        bezierBinding->SetValue(dstBezierProperty->Value, dstBezierProperty->ControlPoint);
    }
}

} // namespace EngineCore

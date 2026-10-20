#include "EulerAnglesRotationTweenController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/ext/quaternion_float.hpp>

namespace EngineCore {

EulerAnglesRotationTweenController::EulerAnglesRotationTweenController()
    : ITweenController()
{
}

EulerAnglesRotationTweenController::~EulerAnglesRotationTweenController()
{
}

void EulerAnglesRotationTweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
    Base::OnTransitionUpdate(deltaTimeSec, transitionParameter);

    if (auto rotationProperty = GetEngineObjectPropertySp<glm::vec3>()) {
        auto srcRotationProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);
        auto dstRotationProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        glm::quat srcQuat = srcRotationProperty->QuatValue;
        glm::quat dstQuat = dstRotationProperty->QuatValue;

        rotationProperty->SetValue(EngineMath::QuatToEulerAngles(EngineMath::SLerpQuat(transitionParameter, srcQuat, dstQuat)));
    }
}

void EulerAnglesRotationTweenController::InitWithPropsInstant(
    const std::shared_ptr<BaseStateProperty>& dstStateProperty,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::InitWithPropsInstant(dstStateProperty, engineObjectProperty);
    if (auto rotationProperty = GetEngineObjectPropertySp<glm::vec3>()) {
        auto dstRotationProperty = std::static_pointer_cast<TweenStateProperty_t>(dstStateProperty);

        rotationProperty->SetValue(dstRotationProperty->Value);
    }
}

void EulerAnglesRotationTweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcProperty,
    const std::shared_ptr<BaseStateProperty>& dstProperty,
    const float transitionDuration,
    const std::shared_ptr<EngineObjectPropertyBase>& engineObjectProperty)
{
    Base::OnTransitionStarted(srcProperty, dstProperty, transitionDuration, engineObjectProperty);

    if (auto rotationProperty = GetEngineObjectPropertySp<glm::vec3>()) {
        auto srcRotationProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::SourceState]);

        rotationProperty->SetValue(srcRotationProperty->Value);
    }
}

void EulerAnglesRotationTweenController::OnTransitionFinished()
{
    if (auto rotationProperty = GetEngineObjectPropertySp<glm::vec3>()) {
        auto dstRotationProperty
            = std::static_pointer_cast<TweenStateProperty_t>(TranstionProperties[(int)StateType::DestinationState]);

        rotationProperty->SetValue(dstRotationProperty->Value);
    }
}

} // namespace EngineCore

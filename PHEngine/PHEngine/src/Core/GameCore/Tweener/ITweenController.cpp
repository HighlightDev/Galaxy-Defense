#include "ITweenController.h"

#include "Core/CommonCore/Assertion.h"
#include "StateProperty.h"

namespace EngineCore {

ITweenController::ITweenController()
{
}

void ITweenController::OnTransitionStarted(
    const std::shared_ptr<BaseStateProperty>& srcStateProperty,
    const std::shared_ptr<BaseStateProperty>& dstStateProperty,
    const float transitionDuration)
{
    mPropertyBinding = srcStateProperty->Binding;
    TranstionProperties[(int)StateType::SourceState] = srcStateProperty;
    TranstionProperties[(int)StateType::DestinationState] = dstStateProperty;
}

void ITweenController::OnTransitionUpdate(const float deltaTimeSec, const float transitionParameter)
{
}

eEnginePropertyBindingType ITweenController::GetControllerPropertyType() const
{
    auto srcProp = TranstionProperties[(int)StateType::SourceState];
    assert(srcProp != nullptr);
    return srcProp->GetStatePropertyType();
}

void ITweenController::InitWithPropsInstant(const std::shared_ptr<BaseStateProperty>& dstStateProperty)
{
}
} // namespace EngineCore
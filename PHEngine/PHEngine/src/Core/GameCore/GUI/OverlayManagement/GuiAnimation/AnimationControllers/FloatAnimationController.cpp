#include "FloatAnimationController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/IAnimatable.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
namespace GUI {
void FloatAnimationController::ProcessAnimation(
    const float animationTimePassed, const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "FloatAnimationController::ProcessAnimation: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Float == propertyType,
        "FloatAnimationController::ProcessAnimation: engineProperty type is not Float");
    const auto floatProperty = std::static_pointer_cast<EngineObjectProperty<float>>(engineProperty);
    ext_assert(
        data.GetSrcValue().type() == typeid(float), "FloatAnimationController::ProcessAnimation: SrcValue type is not float");
    ext_assert(
        data.GetDstValue().type() == typeid(float), "FloatAnimationController::ProcessAnimation: DstValue type is not float");

    if (animationTimePassed >= data.GetAnimationDuration()) {
        mIsAnimationFinished = true;
        floatProperty->SetValue(std::any_cast<float>(data.GetDstValue()));
    } else {
        if (eAnimationInterpolationFunctionType::LINEAR == data.GetAnimationFunctionType()) {
            const float srcValue = floatProperty->GetInitialValue();
            const float dstValue = std::any_cast<float>(data.GetDstValue());
            const float resultValue
                = EngineMath::LerpFloat(animationTimePassed, 0.0f, data.GetAnimationDuration(), srcValue, dstValue);
            floatProperty->SetValue(resultValue);
        } else {
            ext_assert(false, "FloatAnimationController::ProcessAnimation: Unsupported animation function type");
        }
    }
}

void FloatAnimationController::ForceFinishAnimation(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "FloatAnimationController::ForceFinishAnimation: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Float == propertyType,
        "FloatAnimationController::ForceFinishAnimation: engineProperty type is not Float");
    ext_assert(
        data.GetDstValue().type() == typeid(float), "FloatAnimationController::ForceFinishAnimation: DstValue type is not float");
    const auto floatProperty = std::static_pointer_cast<EngineObjectProperty<float>>(engineProperty);

    mIsAnimationFinished = true;
    floatProperty->SetValue(std::any_cast<float>(data.GetDstValue()));
}

void FloatAnimationController::InitWithSrcValues(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "FloatAnimationController::InitWithSrcValues: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Float == propertyType,
        "FloatAnimationController::InitWithSrcValues: engineProperty type is not Float");
    ext_assert(
        data.GetDstValue().type() == typeid(float), "FloatAnimationController::InitWithSrcValues: SrcValue type is not float");
    const auto floatProperty = std::static_pointer_cast<EngineObjectProperty<float>>(engineProperty);
    floatProperty->SetValue(std::any_cast<float>(data.GetSrcValue()));
    mIsAnimationFinished = false;
}

eEnginePropertyType FloatAnimationController::GetPropertyType() const
{
    return eEnginePropertyType::Float;
}
} // namespace GUI

} // namespace EngineCore

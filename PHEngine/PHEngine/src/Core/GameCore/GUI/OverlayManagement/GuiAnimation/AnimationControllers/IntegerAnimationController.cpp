#include "IntegerAnimationController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/IAnimatable.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
namespace GUI {
void IntegerAnimationController::ProcessAnimation(
    const float animationTimePassed, const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "IntegerAnimationController::ProcessAnimation: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Integer == propertyType,
        "IntegerAnimationController::ProcessAnimation: engineProperty type is not Integer");
    ext_assert(
        data.GetSrcValue().type() == typeid(int32_t),
        "IntegerAnimationController::ProcessAnimation: SrcValue type is not int32_t");
    ext_assert(
        data.GetDstValue().type() == typeid(int32_t),
        "IntegerAnimationController::ProcessAnimation: DstValue type is not int32_t");
    const auto intProperty = std::static_pointer_cast<EngineObjectProperty<int32_t>>(engineProperty);

    if (animationTimePassed >= data.GetAnimationDuration()) {
        mIsAnimationFinished = true;
        intProperty->SetValue(std::any_cast<int32_t>(data.GetDstValue()));
    } else {
        if (eAnimationInterpolationFunctionType::LINEAR == data.GetAnimationFunctionType()) {
            const int32_t srcValue = intProperty->GetInitialValue();
            const int32_t dstValue = std::any_cast<int32_t>(data.GetDstValue());
            const int32_t resultValue
                = EngineMath::LerpInt(animationTimePassed, 0.0f, data.GetAnimationDuration(), srcValue, dstValue);
            intProperty->SetValue(resultValue);
        } else {
            ext_assert(false, "IntegerAnimationController::ProcessAnimation: Unsupported animation function type");
        }
    }
}

void IntegerAnimationController::ForceFinishAnimation(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "IntegerAnimationController::ForceFinishAnimation: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Integer == propertyType,
        "IntegerAnimationController::ForceFinishAnimation: engineProperty type is not Integer");
    ext_assert(
        data.GetDstValue().type() == typeid(int32_t),
        "IntegerAnimationController::ForceFinishAnimation: DstValue type is not int32_t");
    const auto intProperty = std::static_pointer_cast<EngineObjectProperty<int32_t>>(engineProperty);

    mIsAnimationFinished = true;
    intProperty->SetValue(std::any_cast<int32_t>(data.GetDstValue()));
}

void IntegerAnimationController::InitWithSrcValues(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "IntegerAnimationController::InitWithSrcValues: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Integer == propertyType,
        "IntegerAnimationController::InitWithSrcValues: engineProperty type is not Integer");
    ext_assert(
        data.GetDstValue().type() == typeid(int32_t),
        "IntegerAnimationController::InitWithSrcValues: DstValue type is not int32_t");
    const auto intProperty = std::static_pointer_cast<EngineObjectProperty<int32_t>>(engineProperty);
    intProperty->SetValue(std::any_cast<int32_t>(data.GetSrcValue()));
    mIsAnimationFinished = false;
}

eEnginePropertyType IntegerAnimationController::GetPropertyType() const
{
    return eEnginePropertyType::Integer;
}
} // namespace GUI

} // namespace EngineCore

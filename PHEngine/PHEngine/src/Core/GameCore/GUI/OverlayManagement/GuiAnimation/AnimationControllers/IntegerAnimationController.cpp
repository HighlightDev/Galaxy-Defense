#include "IntegerAnimationController.h"

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
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Integer == propertyType);
    const auto intProperty = std::static_pointer_cast<EngineObjectProperty<int32_t>>(engineProperty);

    if (animationTimePassed >= data.GetAnimationDuration()) {
        mIsAnimationFinished = true;
        intProperty->SetValue(std::any_cast<int32_t>(data.GetDstValue()));
    } else {
        if (eAnimationInterpolationFunctionType::LINEAR == data.GetAnimationFunctionType()) {
            const int32_t srcValue = std::any_cast<int32_t>(data.GetSrcValue());
            const int32_t dstValue = std::any_cast<int32_t>(data.GetDstValue());
            const int32_t resultValue
                = EngineMath::LerpInt(animationTimePassed, 0.0f, data.GetAnimationDuration(), srcValue, dstValue);
            intProperty->SetValue(resultValue);
        } else {
            assert(false);
        }
    }
}

void IntegerAnimationController::ForceFinishAnimation(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Integer == propertyType);
    const auto intProperty = std::static_pointer_cast<EngineObjectProperty<int32_t>>(engineProperty);

    mIsAnimationFinished = true;
    intProperty->SetValue(std::any_cast<int32_t>(data.GetDstValue()));
}

void IntegerAnimationController::InitWithSrcValues(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Integer == propertyType);
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

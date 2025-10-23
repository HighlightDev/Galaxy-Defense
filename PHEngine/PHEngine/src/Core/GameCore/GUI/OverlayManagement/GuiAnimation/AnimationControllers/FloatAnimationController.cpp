#include "FloatAnimationController.h"

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
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Float == propertyType);
    const auto floatProperty = std::static_pointer_cast<EngineObjectProperty<float>>(engineProperty);
    assert(data.GetSrcValue().type() == typeid(float));
    assert(data.GetDstValue().type() == typeid(float));

    if (animationTimePassed >= data.GetAnimationDuration()) {
        mIsAnimationFinished = true;
        floatProperty->SetValue(std::any_cast<float>(data.GetDstValue()));
    } else {
        if (eAnimationInterpolationFunctionType::LINEAR == data.GetAnimationFunctionType()) {
            const float srcValue = floatProperty->GetValue();
            const float dstValue = std::any_cast<float>(data.GetDstValue());
            const float resultValue
                = EngineMath::LerpFloat(animationTimePassed, 0.0f, data.GetAnimationDuration(), srcValue, dstValue);
            floatProperty->SetValue(resultValue);
        } else {
            assert(false);
        }
    }
}

void FloatAnimationController::ForceFinishAnimation(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Float == propertyType);
    assert(data.GetDstValue().type() == typeid(float));
    const auto floatProperty = std::static_pointer_cast<EngineObjectProperty<float>>(engineProperty);

    mIsAnimationFinished = true;
    floatProperty->SetValue(std::any_cast<float>(data.GetDstValue()));
}

void FloatAnimationController::InitWithSrcValues(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Float == propertyType);
    assert(data.GetDstValue().type() == typeid(float));
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

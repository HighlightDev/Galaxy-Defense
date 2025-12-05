#include "Vec2AnimationController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/IAnimatable.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
namespace GUI {
void Vec2AnimationController::ProcessAnimation(
    const float animationTimePassed, const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "Vec2AnimationController::ProcessAnimation: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Vec2 == propertyType, "Vec2AnimationController::ProcessAnimation: engineProperty type is not Vec2");
    ext_assert(
        eEnginePropertyType::Vec2 == propertyType, "Vec2AnimationController::ProcessAnimation: engineProperty type is not Vec2");
    const auto vec2Property = std::static_pointer_cast<EngineObjectProperty<glm::vec2>>(engineProperty);

    ext_assert(
        data.GetSrcValue().type() == typeid(glm::vec2),
        "Vec2AnimationController::ProcessAnimation: SrcValue type is not glm::vec2");
    ext_assert(
        data.GetDstValue().type() == typeid(glm::vec2),
        "Vec2AnimationController::ProcessAnimation: DstValue type is not glm::vec2");
    if (animationTimePassed >= data.GetAnimationDuration()) {
        mIsAnimationFinished = true;
        vec2Property->SetValue(std::any_cast<glm::vec2>(data.GetDstValue()));
    } else {
        if (eAnimationInterpolationFunctionType::LINEAR == data.GetAnimationFunctionType()) {
            const glm::vec2 srcValue = vec2Property->GetValue();
            const glm::vec2 dstValue = std::any_cast<glm::vec2>(data.GetDstValue());
            const glm::vec2 resultValue
                = EngineMath::LerpVec2(animationTimePassed, 0.0f, data.GetAnimationDuration(), srcValue, dstValue);
            vec2Property->SetValue(resultValue);
        } else {
            ext_assert(false, "Vec2AnimationController::ProcessAnimation: Unsupported animation function type");
        }
    }
}

void Vec2AnimationController::ForceFinishAnimation(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "Vec2AnimationController::ForceFinishAnimation: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Vec2 == propertyType,
        "Vec2AnimationController::ForceFinishAnimation: engineProperty type is not Vec2");
    ext_assert(
        data.GetDstValue().type() == typeid(glm::vec2),
        "Vec2AnimationController::ForceFinishAnimation: DstValue type is not glm::vec2");
    const auto vec2Property = std::static_pointer_cast<EngineObjectProperty<glm::vec2>>(engineProperty);

    mIsAnimationFinished = true;
    vec2Property->SetValue(std::any_cast<glm::vec2>(data.GetDstValue()));
}

void Vec2AnimationController::InitWithSrcValues(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    ext_assert(engineProperty, "Vec2AnimationController::InitWithSrcValues: engineProperty is null");
    const auto propertyType = engineProperty->GetPropertyType();
    ext_assert(
        eEnginePropertyType::Vec2 == propertyType, "Vec2AnimationController::InitWithSrcValues: engineProperty type is not Vec2");
    ext_assert(
        data.GetDstValue().type() == typeid(glm::vec2),
        "Vec2AnimationController::InitWithSrcValues: DstValue type is not glm::vec2");
    const auto vec2Property = std::static_pointer_cast<EngineObjectProperty<glm::vec2>>(engineProperty);
    vec2Property->SetValue(std::any_cast<glm::vec2>(data.GetSrcValue()));
    mIsAnimationFinished = false;
}

eEnginePropertyType Vec2AnimationController::GetPropertyType() const
{
    return eEnginePropertyType::Vec2;
}
} // namespace GUI

} // namespace EngineCore

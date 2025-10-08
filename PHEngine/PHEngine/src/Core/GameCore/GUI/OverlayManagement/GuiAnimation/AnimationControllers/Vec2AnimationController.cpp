#include "Vec2AnimationController.h"

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
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Vec2 == propertyType);
    const auto vec2Property = std::static_pointer_cast<EngineObjectProperty<glm::vec2>>(engineProperty);

    assert(data.GetSrcValue().type() == typeid(glm::vec2));
    assert(data.GetDstValue().type() == typeid(glm::vec2));

    if (animationTimePassed >= data.GetAnimationDuration()) {
        mIsAnimationFinished = true;
        vec2Property->SetValue(std::any_cast<glm::vec2>(data.GetDstValue()));
    } else {
        if (eAnimationInterpolationFunctionType::LINEAR == data.GetAnimationFunctionType()) {
            const glm::vec2 srcValue = std::any_cast<glm::vec2>(data.GetSrcValue());
            const glm::vec2 dstValue = std::any_cast<glm::vec2>(data.GetDstValue());
            const glm::vec2 resultValue
                = EngineMath::LerpVec2(animationTimePassed, 0.0f, data.GetAnimationDuration(), srcValue, dstValue);
            vec2Property->SetValue(resultValue);
        } else {
            assert(false);
        }
    }
}

void Vec2AnimationController::ForceFinishAnimation(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Vec2 == propertyType);
    assert(data.GetDstValue().type() == typeid(glm::vec2));
    const auto vec2Property = std::static_pointer_cast<EngineObjectProperty<glm::vec2>>(engineProperty);

    mIsAnimationFinished = true;
    vec2Property->SetValue(std::any_cast<glm::vec2>(data.GetDstValue()));
}

void Vec2AnimationController::InitWithSrcValues(const AnimationData& data, const std::shared_ptr<IAnimatable>& animatable)
{
    const auto& engineProperty = animatable->GetPropertyByName(data.GetPropertyName());
    assert(engineProperty);
    const auto propertyType = engineProperty->GetPropertyType();
    assert(eEnginePropertyType::Vec2 == propertyType);
    assert(data.GetDstValue().type() == typeid(glm::vec2));
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

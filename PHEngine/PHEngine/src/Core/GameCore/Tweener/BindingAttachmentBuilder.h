#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GameCore/EngineObjectPropertyBindings/AnimationPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/BooleanPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/EulerAnglesRotationPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/FloatPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/Vec2PropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/Vec3PropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/iVec2PropertyBinding.h"

using namespace Resources;

namespace EngineCore {
template<typename EnginePropertyValueType>
std::shared_ptr<EngineObjectProperty<EnginePropertyValueType>>
CastBasePropertyToType(const std::shared_ptr<EngineObjectPropertyBase>& goProperty)
{
    return std::static_pointer_cast<EngineObjectProperty<EnginePropertyValueType>>(goProperty);
}

struct BindingAttachmentBuilder {
    static void SetAttachment(
        const std::shared_ptr<EngineObject>& gameObjectSp,
        const std::shared_ptr<PropertyBinding>& bindingSp,
        const std::string& gameObjectPropertyName)
    {
        bindingSp->EngineObjectName = gameObjectSp->GetEngineObjectName();
        bindingSp->EngineObjectPropertyName = gameObjectPropertyName;

        switch (bindingSp->GetBindingType()) {
        case eEnginePropertyBindingType::Animation: {
            const auto animationBinding = std::static_pointer_cast<AnimationPropertyBinding>(bindingSp);
            auto propSrcTime = CastBasePropertyToType<float>(gameObjectSp->GetEnginePropertyByName("SrcAnimTime").lock());
            auto propDstTime = CastBasePropertyToType<float>(gameObjectSp->GetEnginePropertyByName("DstAnimTime").lock());
            auto propTransitionValue
                = CastBasePropertyToType<float>(gameObjectSp->GetEnginePropertyByName("AnimTransitionValue").lock());
            auto propIsTransition
                = CastBasePropertyToType<bool>(gameObjectSp->GetEnginePropertyByName("bAnimTransitionEnabled").lock());
            auto propSrcName = CastBasePropertyToType<std::string>(gameObjectSp->GetEnginePropertyByName("SrcAnimName").lock());
            auto propDstName = CastBasePropertyToType<std::string>(gameObjectSp->GetEnginePropertyByName("DstAnimName").lock());

            animationBinding->SetBindingProperties(
                propSrcName, propDstName, propSrcTime, propDstTime, propIsTransition, propTransitionValue);
            break;
        }
        case eEnginePropertyBindingType::FloatScalar: {
            const auto floatBinding = std::static_pointer_cast<FloatPropertyBinding>(bindingSp);
            auto gameObjectProperty
                = CastBasePropertyToType<float>(gameObjectSp->GetEnginePropertyByName(gameObjectPropertyName).lock());
            floatBinding->SetEngineObjectProperty(gameObjectProperty);
            break;
        }
        case eEnginePropertyBindingType::EulerAnglesRotation: {
            const auto rotationBinding = std::static_pointer_cast<EulerAnglesRotationPropertyBinding>(bindingSp);
            auto gameObjectProperty
                = CastBasePropertyToType<glm::vec3>(gameObjectSp->GetEnginePropertyByName(gameObjectPropertyName).lock());
            rotationBinding->SetEngineObjectProperty(gameObjectProperty);
            break;
        }
        case eEnginePropertyBindingType::Boolean: {
            const auto booleanBinding = std::static_pointer_cast<BooleanPropertyBinding>(bindingSp);
            auto gameObjectProperty
                = CastBasePropertyToType<bool>(gameObjectSp->GetEnginePropertyByName(gameObjectPropertyName).lock());
            booleanBinding->SetEngineObjectProperty(gameObjectProperty);
            break;
        }
        case eEnginePropertyBindingType::Vec3: {
            const auto vec3Binding = std::static_pointer_cast<EulerAnglesRotationPropertyBinding>(bindingSp);
            auto gameObjectProperty
                = CastBasePropertyToType<glm::vec3>(gameObjectSp->GetEnginePropertyByName(gameObjectPropertyName).lock());
            vec3Binding->SetEngineObjectProperty(gameObjectProperty);
            break;
        }
        case eEnginePropertyBindingType::iVec2: {
            const auto ivec2Binding = std::static_pointer_cast<iVec2PropertyBinding>(bindingSp);
            const auto& gameObjectProperty
                = CastBasePropertyToType<glm::ivec2>(gameObjectSp->GetEnginePropertyByName(gameObjectPropertyName).lock());
            ivec2Binding->SetEngineObjectProperty(gameObjectProperty);
            break;
        }
        case eEnginePropertyBindingType::Vec2: {
            const auto vec2Binding = std::static_pointer_cast<Vec2PropertyBinding>(bindingSp);
            const auto& gameObjectProperty
                = CastBasePropertyToType<glm::vec2>(gameObjectSp->GetEnginePropertyByName(gameObjectPropertyName).lock());
            vec2Binding->SetEngineObjectProperty(gameObjectProperty);
            break;
        }
        default:
            assert(false); // "unknown bindingSp type."
            break;
        }
    }
};
} // namespace EngineCore
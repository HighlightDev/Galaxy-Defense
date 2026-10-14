#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "PropertyBinding.h"

#include <glm/vec2.hpp>

#include <string>

namespace EngineCore {

struct iVec2PropertyBinding : public PropertyBinding {
private:
    std::weak_ptr<EngineObjectProperty<glm::ivec2>> mGoPropertyWp;

public:
    iVec2PropertyBinding(const std::string& bindingName)
        : PropertyBinding(bindingName)
        , mGoPropertyWp()
    {
    }

    void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<glm::ivec2>>& gameObjectProperty)
    {
        ext_assert(gameObjectProperty, "iVec2PropertyBinding::SetEngineObjectProperty: gameObjectProperty is null");
        mGoPropertyWp = gameObjectProperty;
        bPropertyConnected = true;
    }

    void SetValue(const glm::ivec2& value)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: iVec2PropertyBinding::SetValue: Property. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    glm::ivec2 GetValue() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: iVec2PropertyBinding::GetValue is used. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            return propertySp->GetValue();
        }
        return glm::ivec2();
    }

    eEnginePropertyBindingType GetBindingType() const override
    {
        return eEnginePropertyBindingType::iVec2;
    }
};
} // namespace EngineCore

#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "PropertyBinding.h"

#include <string>

namespace EngineCore {

struct BooleanPropertyBinding : public PropertyBinding {
private:
    std::weak_ptr<EngineObjectProperty<bool>> mGoPropertyWp;

public:
    BooleanPropertyBinding(const std::string& bindingName)
        : PropertyBinding(bindingName)
        , mGoPropertyWp()
    {
    }

    void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<bool>>& gameObjectProperty)
    {
        ext_assert(gameObjectProperty, "BooleanPropertyBinding::SetEngineObjectProperty: gameObjectProperty is null");
        mGoPropertyWp = gameObjectProperty;
        bPropertyConnected = true;
    }

    void SetValue(const bool value)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: BooleanPropertyBinding::SetValue: Property. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    bool GetValue() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: BooleanPropertyBinding::GetValue: Property. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            return propertySp->GetValue();
        }
        return false;
    }

    eEnginePropertyBindingType GetBindingType() const override
    {
        return eEnginePropertyBindingType::Boolean;
    }
};
} // namespace EngineCore

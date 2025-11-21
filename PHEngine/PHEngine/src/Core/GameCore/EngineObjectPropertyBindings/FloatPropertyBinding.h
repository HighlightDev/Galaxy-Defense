#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "PropertyBinding.h"

#include <string>

namespace EngineCore {

struct FloatPropertyBinding : public PropertyBinding {
private:
    std::weak_ptr<EngineObjectProperty<float>> mGoPropertyWp;

public:
    FloatPropertyBinding(const std::string& bindingName)
        : PropertyBinding(bindingName)
        , mGoPropertyWp()
    {
    }

    void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<float>> gameObjectProperty)
    {
        ext_assert(gameObjectProperty, "FloatPropertyBinding::SetEngineObjectProperty: gameObjectProperty is nullptr");
        mGoPropertyWp = gameObjectProperty;
        bPropertyConnected = true;
    }

    void SetValue(float value)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: FloatPropertyBinding::SetValue: Property is not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    float GetValue() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: FloatPropertyBinding::GetValue: Property is not connected. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            return propertySp->GetValue();
        }
        return 0.0f;
    }

    eEnginePropertyBindingType GetBindingType() const override
    {
        return eEnginePropertyBindingType::FloatScalar;
    }
};
} // namespace EngineCore

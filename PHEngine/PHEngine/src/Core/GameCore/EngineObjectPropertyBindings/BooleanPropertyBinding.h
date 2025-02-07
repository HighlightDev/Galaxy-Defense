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
        assert(gameObjectProperty);
        mGoPropertyWp = gameObjectProperty;
        bPropertyConnected = true;
    }

    void SetValue(const bool value)
    {
        assert(bPropertyConnected);
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    bool GetValue() const
    {
        assert(bPropertyConnected);
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

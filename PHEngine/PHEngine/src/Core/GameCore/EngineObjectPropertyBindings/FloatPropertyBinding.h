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
        assert(gameObjectProperty);
        mGoPropertyWp = gameObjectProperty;
        bPropertyConnected = true;
    }

    void SetValue(float value)
    {
        assert(bPropertyConnected);
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    float GetValue() const
    {
        assert(bPropertyConnected);
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

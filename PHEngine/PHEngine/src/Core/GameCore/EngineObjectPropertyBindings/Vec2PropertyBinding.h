#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "PropertyBinding.h"

#include <glm/vec2.hpp>

#include <string>

namespace EngineCore {

struct Vec2PropertyBinding : public PropertyBinding {
private:
    std::weak_ptr<EngineObjectProperty<glm::vec2>> mGoPropertyWp;

public:
    Vec2PropertyBinding(const std::string& bindingName)
        : PropertyBinding(bindingName)
        , mGoPropertyWp()
    {
    }

    void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<glm::vec2>> gameObjectProperty)
    {
        assert(gameObjectProperty);
        mGoPropertyWp = gameObjectProperty;
        bPropertyConnected = true;
    }

    void SetValue(const glm::vec2& value)
    {
        assert(bPropertyConnected);
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    glm::vec2 GetValue() const
    {
        assert(bPropertyConnected);
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            return propertySp->GetValue();
        }
        return glm::vec2();
    }

    eEnginePropertyBindingType GetBindingType() const override
    {
        return eEnginePropertyBindingType::Vec2;
    }
};
} // namespace EngineCore

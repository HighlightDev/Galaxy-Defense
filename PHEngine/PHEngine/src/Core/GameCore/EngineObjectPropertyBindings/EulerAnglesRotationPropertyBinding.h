#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "PropertyBinding.h"

#include <glm/vec3.hpp>

#include <string>

namespace EngineCore {

struct EulerAnglesRotationPropertyBinding : public PropertyBinding {
private:
    std::weak_ptr<EngineObjectProperty<glm::vec3>> mGoPropertyWp;

public:
    EulerAnglesRotationPropertyBinding(const std::string& bindingName)
        : PropertyBinding(bindingName)
        , mGoPropertyWp()
    {
    }

    void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<glm::vec3>>& engineGoProperty)
    {
        assert(engineGoProperty);
        mGoPropertyWp = engineGoProperty;
        bPropertyConnected = true;
    }

    void SetValue(const glm::vec3& value)
    {
        assert(bPropertyConnected);
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            propertySp->SetValue(value);
        }
    }

    glm::vec3 GetValue() const
    {
        assert(bPropertyConnected);
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            return propertySp->GetValue();
        }
        return glm::vec3();
    }

    eEnginePropertyBindingType GetBindingType() const override
    {
        return eEnginePropertyBindingType::EulerAnglesRotation;
    }
};
} // namespace EngineCore

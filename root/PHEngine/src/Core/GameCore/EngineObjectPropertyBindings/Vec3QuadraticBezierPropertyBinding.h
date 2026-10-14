#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "PropertyBinding.h"

#include <glm/vec3.hpp>

#include <string>

namespace EngineCore {

struct Vec3QuadraticBezierPropertyBinding : public PropertyBinding {
private:
    std::weak_ptr<EngineObjectProperty<glm::vec3>> mGoPropertyWp;
    std::weak_ptr<EngineObjectProperty<glm::vec3>> mGoControlPointPropertyWp;

public:
    Vec3QuadraticBezierPropertyBinding(const std::string& bindingName)
        : PropertyBinding(bindingName)
        , mGoPropertyWp()
    {
    }

    void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<glm::vec3>>& engineGoProperty)
    {
        ext_assert(
            engineGoProperty, "EngineObjectProperty is null in Vec3QuadraticBezierPropertyBinding::SetEngineObjectProperty");
        mGoPropertyWp = engineGoProperty;
        bPropertyConnected = true;
    }

    void SetValue(const glm::vec3& value, const glm::vec3& controlPoint)
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: Vec3QuadraticBezierPropertyBinding::SetValue: Property. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        bBindingInitialized = true;
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            propertySp->SetValue(value);
        }
        if (const auto& controlPointPropertySp = mGoControlPointPropertyWp.lock()) {
            controlPointPropertySp->SetValue(controlPoint);
        }
    }

    glm::vec3 GetValue() const
    {
        if (!bPropertyConnected && !bBindingInitialized) {
            LogInfo(
                "Warning: Vec3QuadraticBezierPropertyBinding::GetValue is used. BindingName: " + BindingName
                + ", EngineObjectName: " + EngineObjectName + ", EngineObjectPropertyName: " + EngineObjectPropertyName);
        }
        if (const auto& propertySp = mGoPropertyWp.lock()) {
            return propertySp->GetValue();
        }
        return glm::vec3();
    }

    glm::vec3 GetControlPoint() const
    {
        if (const auto& controlPointPropertySp = mGoControlPointPropertyWp.lock()) {
            return controlPointPropertySp->GetValue();
        }
        return glm::vec3();
    }

    eEnginePropertyBindingType GetBindingType() const override
    {
        return eEnginePropertyBindingType::Vec3QuadraticBezier;
    }
}; // namespace EngineCore
} // namespace EngineCore

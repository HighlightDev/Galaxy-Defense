#pragma once

#include "BindingMaterialProperty.h"

namespace Graphics {

struct FloatBindingMaterialProperty : public BindingMaterialProperty {
public:
    explicit FloatBindingMaterialProperty(const std::string& propertyName, const std::shared_ptr<IMaterial>& ownerMaterial)
        : BindingMaterialProperty(propertyName, ownerMaterial)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY;
    }

    eEnginePropertyType GetBindingEnginePropertyType() const override
    {
        return eEnginePropertyType::Float;
    }

    void SetValueToUniform(ActiveBindedState& activeBindedState, Uniform uniform, const int32_t propertyIndex) const override
    {
        ext_assert(
            false,
            "FloatBindingMaterialProperty::SetValueToUniform: this code should not be called, use proxy on render thread part");
    } // this code should not be called, use proxy on render thread part

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    float GetValue() const
    {
        return GetEngineObjectPropertyValue<float>();
    }
};
} // namespace Graphics

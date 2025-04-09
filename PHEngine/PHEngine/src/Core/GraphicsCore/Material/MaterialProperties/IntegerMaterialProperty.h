#pragma once

#include "MaterialProperty.h"

namespace Graphics {

struct IntegerMaterialProperty : public MaterialProperty {
    using MaterialPropertyValueType = int32_t;

private:
    MaterialPropertyValueType m_value;

public:
    IntegerMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value(propertyValue)
    {
    }

    IntegerMaterialProperty(const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value(0)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::INTEGER_PROPERTY;
    }

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    void SetValueToUniform(ActiveBindedState& activeBindedState, Uniform uniform, const int32_t propertyIndex) const override
    {
        uniform.LoadUniform(m_value);
    }

    inline void SetValue(MaterialPropertyValueType value)
    {
        m_value = value;
    }

    inline MaterialPropertyValueType GetValue() const
    {
        return m_value;
    }
};
} // namespace Graphics

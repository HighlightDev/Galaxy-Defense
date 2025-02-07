#pragma once

#include "MaterialProperty.h"

#include <glm/vec3.hpp>

namespace Graphics {

struct Vec3MaterialProperty : public MaterialProperty {
    using MaterialPropertyValueType = glm::vec3;

private:
    MaterialPropertyValueType m_value;

public:
    Vec3MaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value(propertyValue)
    {
    }

    Vec3MaterialProperty(const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value()
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::VEC3_PROPERTY;
    }

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
    {
        uniform.LoadUniform(m_value);
    }

    inline void SetValue(const MaterialPropertyValueType& value)
    {
        m_value = value;
    }

    inline const MaterialPropertyValueType& GetValue() const
    {
        return m_value;
    }
};
} // namespace Graphics

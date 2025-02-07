#pragma once

#include "MaterialProperty.h"

#include <glm/vec2.hpp>

namespace Graphics {

struct iVec2MaterialProperty : public MaterialProperty {
    using MaterialPropertyValueType = glm::ivec2;

private:
    MaterialPropertyValueType m_value;

public:
    iVec2MaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value(propertyValue)
    {
    }

    iVec2MaterialProperty(const std::string& propertyName)
        : MaterialProperty(propertyName)
        , m_value()
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::IVEC2_PROPERTY;
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

#pragma once

#include "BindingMaterialProperty.h"

#include <glm/vec3.hpp>

namespace Graphics {

struct Vec3BindingMaterialProperty : public BindingMaterialProperty {
public:
    explicit Vec3BindingMaterialProperty(const std::string& propertyName, const std::shared_ptr<IMaterial>& ownerMaterial)
        : BindingMaterialProperty(propertyName, ownerMaterial)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::VEC3_BINDING_PROPERTY;
    }

    eEnginePropertyType GetBindingEnginePropertyType() const override
    {
        return eEnginePropertyType::Vec3;
    }

    void SetValueToUniform(ActiveBindedState& activeBindedState, Uniform uniform, const int32_t propertyIndex) const override
    {
        ext_assert(
            false,
            "Vec3BindingMaterialProperty::SetValueToUniform: this code should not be called, use proxy on render thread part");
    } // this code should not be called, use proxy on render thread part

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    glm::vec3 GetValue() const
    {
        return GetEngineObjectPropertyValue<glm::vec3>();
    }
};
} // namespace Graphics

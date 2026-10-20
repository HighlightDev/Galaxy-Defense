#pragma once

#include "BindingMaterialProperty.h"

#include <glm/vec2.hpp>

namespace Graphics {

struct iVec2BindingMaterialProperty : public BindingMaterialProperty {
public:
    explicit iVec2BindingMaterialProperty(const std::string& propertyName, const std::shared_ptr<IMaterial>& ownerMaterial)
        : BindingMaterialProperty(propertyName, ownerMaterial)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::IVEC2_BINDING_PROPERTY;
    }

    eEnginePropertyType GetBindingEnginePropertyType() const override
    {
        return eEnginePropertyType::iVec2;
    }

    void SetValueToUniform(ActiveBindedState& activeBindedState, Uniform uniform, const int32_t propertyIndex) const override
    {
        ext_assert(
            false,
            "iVec2BindingMaterialProperty::SetValueToUniform: this code should not be called, use proxy on render thread part");
    } // this code should not be called, use proxy on render thread part

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    glm::ivec2 GetValue() const
    {
        return GetEngineObjectPropertyValue<glm::ivec2>();
    }
};
} // namespace Graphics

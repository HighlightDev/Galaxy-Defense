#pragma once

#include "BindingMaterialProperty.h"
#include "Core/GameCore/EngineObjectPropertyBindings/Vec3PropertyBinding.h"

using namespace EngineCore;

namespace Graphics {

struct Vec3BindingMaterialProperty : public BindingMaterialProperty

{
    using MaterialPropertyValueType = std::shared_ptr<Vec3PropertyBinding>;

public:
    Vec3BindingMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : BindingMaterialProperty(propertyValue, propertyName)
    {
    }

    Vec3BindingMaterialProperty(const std::string& propertyName)
        : BindingMaterialProperty(propertyName)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::VEC3_BINDING_PROPERTY;
    }

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    void SetValueToUniform(ActiveBindedState& activeBindedState, Uniform uniform, const int32_t propertyIndex) const override
    {
        assert(false);
    } // this code should not be called, use proxy on render thread part

    void SetValue(MaterialPropertyValueType value)
    {
        mPropertyBinding = value;
    }

    glm::vec3 GetValue() const
    {
        return std::static_pointer_cast<Vec3PropertyBinding>(mPropertyBinding)->GetValue();
    }
};
} // namespace Graphics

#pragma once

#include "BindingMaterialProperty.h"
#include "Core/GameCore/EngineObjectPropertyBindings/FloatPropertyBinding.h"

using namespace EngineCore;

namespace Graphics {

struct FloatBindingMaterialProperty : public BindingMaterialProperty

{
    using MaterialPropertyValueType = std::shared_ptr<FloatPropertyBinding>;

public:
    FloatBindingMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : BindingMaterialProperty(propertyValue, propertyName)
    {
    }

    FloatBindingMaterialProperty(const std::string& propertyName)
        : BindingMaterialProperty(propertyName)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY;
    }

    void SetValueToUniform(ActiveBindedState& activeBindedState, Uniform uniform, const int32_t propertyIndex) const override
    {
        assert(false);
    } // this code should not be called, use proxy on render thread part

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    void SetValue(MaterialPropertyValueType value)
    {
        mPropertyBinding = value;
    }

    float GetValue() const
    {
        return std::static_pointer_cast<FloatPropertyBinding>(mPropertyBinding)->GetValue();
    }
};
} // namespace Graphics

#pragma once

#include "BindingMaterialProperty.h"
#include "Core/GameCore/EngineObjectPropertyBindings/iVec2PropertyBinding.h"

using namespace EngineCore;

namespace Graphics {

struct iVec2BindingMaterialProperty : public BindingMaterialProperty

{
    using MaterialPropertyValueType = std::shared_ptr<iVec2PropertyBinding>;

public:
    iVec2BindingMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : BindingMaterialProperty(propertyValue, propertyName)
    {
    }

    iVec2BindingMaterialProperty(const std::string& propertyName)
        : BindingMaterialProperty(propertyName)
    {
    }

    eMaterialPropertyType GetPropertyType() const override
    {
        return MaterialProperty::eMaterialPropertyType::IVEC2_BINDING_PROPERTY;
    }

    void SetValueToUniformArray(const UniformArray& uniformArray) const override
    {
    }

    void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
    {
        assert(false);
    } // this code should not be called, use proxy on render thread part

    void SetValue(MaterialPropertyValueType value)
    {
        mPropertyBinding = value;
    }

    glm::ivec2 GetValue() const
    {
        return std::static_pointer_cast<iVec2PropertyBinding>(mPropertyBinding)->GetValue();
    }
};
} // namespace Graphics

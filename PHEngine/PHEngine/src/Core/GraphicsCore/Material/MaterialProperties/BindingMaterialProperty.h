#pragma once

#include "Core/GameCore/EngineObjectPropertyBindings/PropertyBinding.h"
#include "MaterialProperty.h"

using namespace EngineCore;

namespace Graphics {

struct BindingMaterialProperty : public MaterialProperty {
    using MaterialPropertyValueType = std::shared_ptr<PropertyBinding>;

protected:
    MaterialPropertyValueType mPropertyBinding;

public:
    BindingMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
        : MaterialProperty(propertyName)
        , mPropertyBinding(propertyValue)
    {
    }

    BindingMaterialProperty(const std::string& propertyName)
        : MaterialProperty(propertyName)
        , mPropertyBinding()
    {
    }

    std::shared_ptr<PropertyBinding> GetMaterialBinding() const
    {
        return mPropertyBinding;
    }
};
} // namespace Graphics
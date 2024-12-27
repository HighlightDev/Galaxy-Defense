#pragma once

#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

#include <algorithm>

namespace Graphics
{

    struct DynamicMaterialProperty
    {
        enum class eDynamicMaterialPropertyType
        {
            FloatProperty,
            iVec2Property,
            Vec2Property,
            Vec3Property,
            InstancedFloatProperty
        };

    protected:
        std::string mPropertyName;

        std::vector<std::shared_ptr<MaterialProperty>> mInternalDynamicMaterialProperties;

    public:
        DynamicMaterialProperty(const std::string &propertyName)
            : mPropertyName(propertyName)
        {
        }

        virtual eDynamicMaterialPropertyType GetPropertyType() const = 0;

        virtual void UpdateStaticPropertyWithDynamicValue(const std::shared_ptr<MaterialProperty> &staticProperty) = 0;

        void SetInternalDynamicMaterialProperties(std::vector<std::shared_ptr<MaterialProperty>> &&internalDynamicMaterialProperties)
        {
            mInternalDynamicMaterialProperties = internalDynamicMaterialProperties;
        }

        std::shared_ptr<MaterialProperty> TryGetInternalMaterialPropertyByName(const std::string &propertyName) const
        {
            auto propertyIt = std::find_if(mInternalDynamicMaterialProperties.begin(), mInternalDynamicMaterialProperties.end(),
                                           [&](const auto &dynamicProperty)
                                           { return propertyName == dynamicProperty->GetPropertyName(); });

            if (propertyIt != mInternalDynamicMaterialProperties.end())
                return *propertyIt;
            else
                return nullptr;
        }

        std::string GetPropertyName() const
        {
            return mPropertyName;
        }
    };
}

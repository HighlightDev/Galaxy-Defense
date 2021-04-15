#pragma once
#include "MaterialProperty.h"
#include "DynamicMaterialOperations/MaterialOperation.h"

#include <algorithm>

namespace Graphics {

   struct DynamicFloatMaterialProperty
      : public MaterialProperty
   {
      using MaterialPropertyValueType = std::shared_ptr<MaterialStartNode>;

   private:

      MaterialPropertyValueType m_value;

      std::vector<std::shared_ptr<MaterialProperty>> mInternalDynamicMaterialProperties;

   public:

      DynamicFloatMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
         : MaterialProperty(propertyName)
         , m_value(propertyValue)
      {
      }

      DynamicFloatMaterialProperty(const std::string& propertyName)
         : MaterialProperty(propertyName)
         , m_value()
      {
      }

      virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override { assert(false); } // this code should not be called, use proxy on render thread part 

      virtual MaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY;
      }

      inline void SetValue(MaterialPropertyValueType value) {
         m_value = value;
      }

      inline float GetValue() const {
         auto value = m_value->GetValue();
         return value;
      }

      void SetInternalDynamicMaterialProperties(std::vector<std::shared_ptr<MaterialProperty>>&& internalDynamicMaterialProperties)
      {
         mInternalDynamicMaterialProperties = internalDynamicMaterialProperties;
      }

      std::shared_ptr<MaterialProperty> TryGetInternalMaterialPropertyByName(const std::string& propertyName) const 
      {
         auto propertyIt = std::find_if(mInternalDynamicMaterialProperties.begin(), mInternalDynamicMaterialProperties.end(),
            [&](const auto& dynamicProperty) { return propertyName == dynamicProperty->GetPropertyName(); });

         if (propertyIt != mInternalDynamicMaterialProperties.end())
            return *propertyIt;
         else
            return nullptr;
      }

   };
}


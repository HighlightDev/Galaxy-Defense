#pragma once

#include "MaterialProperty.h"

namespace Graphics {

   struct FloatMaterialProperty
      : public MaterialProperty
   {
      using MaterialPropertyValueType = float;

   private:

      MaterialPropertyValueType m_value;

   public:

      FloatMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
         : MaterialProperty(propertyName)
         , m_value(propertyValue)
      {
      }

      FloatMaterialProperty(const std::string& propertyName)
         : MaterialProperty(propertyName)
         , m_value(0.0f)
      {
      }

      eMaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY;
      }

      void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
      {
         uniform.LoadUniform(m_value);
      }

      inline void SetValue(MaterialPropertyValueType value) {
         m_value = value;
      }

      inline float GetValue() const {
         return m_value;
      }

   };
}

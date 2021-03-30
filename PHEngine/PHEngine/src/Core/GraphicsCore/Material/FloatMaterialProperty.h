#pragma once

#include "MaterialProperty.h"

struct FloatMaterialProperty
   : public MaterialProperty
{
   using MaterialPropertyValueType = float;

private:

   MaterialPropertyValueType m_value;

public:

   FloatMaterialProperty(MaterialPropertyValueType propertyValue)
      : MaterialProperty()
      , m_value(propertyValue)
   {
   }

   FloatMaterialProperty()
      : MaterialProperty()
      , m_value(0.0f)
   {
   }

   virtual MaterialPropertyType GetMaterialPropertyType() const override
   {
      return MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY;
   }

   virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
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

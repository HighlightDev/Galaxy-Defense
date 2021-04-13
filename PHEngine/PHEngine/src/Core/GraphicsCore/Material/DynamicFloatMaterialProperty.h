#pragma once
#pragma once

#include "MaterialProperty.h"
#include "DynamicMaterialOperations/MaterialOperation.h"

using namespace Graphics;

struct DynamicFloatMaterialProperty
   : public MaterialProperty
{
   using MaterialPropertyValueType = std::shared_ptr<MaterialStartNode>;

private:

   MaterialPropertyValueType m_value;

public:

   DynamicFloatMaterialProperty(MaterialPropertyValueType propertyValue)
      : MaterialProperty()
      , m_value(propertyValue)
   {
   }

   DynamicFloatMaterialProperty()
      : MaterialProperty()
      , m_value()
   {
   }

   virtual MaterialPropertyType GetMaterialPropertyType() const override
   {
      return MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY;
   }

   virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
   {
      auto value = m_value->GetValue();
      uniform.LoadUniform(value);
   }

   inline void SetValue(MaterialPropertyValueType value) {
      m_value = value;
   }

   inline float GetValue() const {
      auto value = m_value->GetValue();
      return value;
   }

};


#pragma once

#include "MaterialProperty.h"
#include "MaterialPropertyBindings/FloatMaterialBinding.h"

namespace Graphics {

   struct FloatBindingMaterialProperty
      : public MaterialProperty
   {
      using MaterialPropertyValueType = std::shared_ptr<FloatMaterialPropertyBinding>;

   private:

      MaterialPropertyValueType m_value;

   public:

      FloatBindingMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
         : MaterialProperty(propertyName)
         , m_value(propertyValue)
      {
      }

      FloatBindingMaterialProperty(const std::string& propertyName)
         : MaterialProperty(propertyName)
         , m_value()
      {
      }

      virtual MaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::MaterialPropertyType::FLOAT_BINDING_PROPERTY;
      }

      virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override { assert(false); } // this code should not be called, use proxy on render thread part 

      inline void SetValue(MaterialPropertyValueType value) {
         m_value = value;
      }

      inline float GetValue() const {
         return *m_value->Value;
      }

   };
}

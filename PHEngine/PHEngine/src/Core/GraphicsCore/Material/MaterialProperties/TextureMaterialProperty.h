#pragma once

#include "MaterialProperty.h"

namespace Graphics {

   struct TextureMaterialProperty
      : public MaterialProperty
   {
      using MaterialPropertyValueType = std::shared_ptr<ITexture>;

   private:

      MaterialPropertyValueType m_value;

   public:

      TextureMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
         : MaterialProperty(propertyName)
         , m_value(propertyValue)
      {
      }

      TextureMaterialProperty(const std::string& propertyName)
         : MaterialProperty(propertyName)
      {
      }

      virtual eMaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY;
      }

      virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
      {
         int32_t slot = 10 + propertyIndex;
         if (m_value)
         {
            m_value->BindTexture(slot);
            uniform.LoadUniform(slot);
         }
      }

      inline void SetValue(MaterialPropertyValueType value) {
         m_value = value;
      }

      inline void SetValue(ITexture* value) {
         m_value = std::shared_ptr<ITexture>(value);
      }

      inline std::shared_ptr<ITexture> GetValue() const {
         return m_value;
      }
   };
}
#pragma once

#include <vector>

#include "MaterialProperty.h"

namespace Graphics
{
   struct InstancedFloatMaterialProperty
       : public MaterialProperty
   {
   private:
      std::vector<float> m_values;

   public:
      InstancedFloatMaterialProperty(std::vector<float> propertyValue, const std::string &propertyName)
          : MaterialProperty(propertyName),
            m_values(std::move(propertyValue))
      {
      }

      InstancedFloatMaterialProperty(const std::string &propertyName)
          : MaterialProperty(propertyName),
            m_values()
      {
      }

      eMaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::eMaterialPropertyType::FLOAT_INSTANCED_PROPERTY;
      }

      void SetValueToUniformArray(const UniformArray &uniformArray) const override
      {
         int32_t index = 0;
         for (const auto value : m_values)
         {
            uniformArray.LoadUniform(index++, value);
         }
      }

      void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
      {
         assert(false);
      }

      inline void SetValue(const std::vector<float> &value)
      {
         m_values = value;
      }

      const std::vector<float> &GetValue() const
      {
         return m_values;
      }
   };
}

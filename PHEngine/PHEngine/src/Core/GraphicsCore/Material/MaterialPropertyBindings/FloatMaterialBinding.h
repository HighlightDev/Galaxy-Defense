#pragma once

#include <string>

#include "MaterialPropertyBinding.h"

namespace Graphics {

   struct FloatMaterialPropertyBinding
      : public MaterialPropertyBinding
   {
      float* Value;

      FloatMaterialPropertyBinding(const std::string& bindingName, float* value)
         : MaterialPropertyBinding(bindingName)
         , Value(value)
      {
      }

      FloatMaterialPropertyBinding(const std::string& bindingName)
         : MaterialPropertyBinding(bindingName)
         , Value(nullptr)
      {
      }

      void SetBindingProperty(float* value)
      {
         Value = value;
      }

      virtual eMaterialBindingType GetMaterialBindingType() const override
      {
         return eMaterialBindingType::FLOAT;
      }
   };
}

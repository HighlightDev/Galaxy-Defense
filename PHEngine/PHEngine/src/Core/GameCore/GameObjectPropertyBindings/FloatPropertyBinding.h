#pragma once

#include <string>

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"

namespace Game {

   struct FloatPropertyBinding
      : public PropertyBinding
   {
   private:
      //todo: refactor raw pointers!!!!!!
      float* Value;

   public:

      FloatPropertyBinding(const std::string& bindingName, float* value)
         : PropertyBinding(bindingName)
         , Value(value)
      {
      }

      FloatPropertyBinding(const std::string& bindingName)
         : PropertyBinding(bindingName)
         , Value(nullptr)
      {
      }

      void SetValuePtr(float* value) 
      {
         Value = value;
         bValueSet = true;
      }

      void SetValue(float value) {
         *Value = value;
         bValueSet = true;
      }

      float GetValue() const {
         assert(bValueSet);
         return *Value;
      }

      virtual eBindingType GetBindingType() const override
      {
         return eBindingType::FLOAT;
      }
   };
}

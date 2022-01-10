#pragma once

#include <string>

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"

namespace Game {

   struct FloatPropertyBinding
      : public PropertyBinding
   {
   private:
      
      propertyPtr_t<float> Value;

   public:

      FloatPropertyBinding(const std::string& bindingName, Game::propertyPtr_t<float> value)
         : PropertyBinding(bindingName)
         , Value(value)
      {
      }

      FloatPropertyBinding(const std::string& bindingName)
         : PropertyBinding(bindingName)
         , Value(nullptr)
      {
      }

      void SetValuePtr(Game::propertyPtr_t<float> value)
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

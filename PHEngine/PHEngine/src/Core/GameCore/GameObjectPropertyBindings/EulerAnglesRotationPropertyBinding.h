#pragma once

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game {

   struct EulerAnglesRotationPropertyBinding
      : public PropertyBinding
   {
   private:
      
      propertyPtr_t<glm::vec3> Value;

   public:

      EulerAnglesRotationPropertyBinding(const std::string& bindingName, Game::propertyPtr_t<glm::vec3> value)
         : PropertyBinding(bindingName)
         , Value(value)
      {
      }

      EulerAnglesRotationPropertyBinding(const std::string& bindingName)
         : PropertyBinding(bindingName)
         , Value(nullptr)
      {
      }

      void SetValuePtr(Game::propertyPtr_t<glm::vec3> value)
      {
         Value = value;
         bValueSet = true;
      }

      void SetValue(const glm::vec3& value) {
         *Value = value;
         bValueSet = true;
      }

      glm::vec3 GetValue() const {
         assert(bValueSet);
         return *Value;
      }

      virtual eBindingType GetBindingType() const override
      {
         return eBindingType::EulerAnglesRotation;
      }
   };
}

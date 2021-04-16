#pragma once

#include <string>

namespace Game {

   enum class eBindingType
   {
      FLOAT,
      ANIMATION,
   };

   struct PropertyBinding
   {
   protected:

      bool bValueSet = false;

   public:

      std::string BindingName;
      std::string GameObjectName;
      std::string GameObjectPropertyName;

      PropertyBinding(const std::string& bindingName)
         : BindingName(bindingName)
         , GameObjectName("")
         , GameObjectPropertyName("")
      {
      }

      virtual eBindingType GetBindingType() const = 0;
   };
}
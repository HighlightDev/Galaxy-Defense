#pragma once

#include <string>
#include <memory>

namespace Game {

   enum class eBindingType
   {
      FLOAT,
      ANIMATION,
   };

   template <typename PtrType>
   using propertyPtr_t = std::shared_ptr<PtrType>;

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
#pragma once

#include <string>
#include <memory>

namespace EngineCore {

   enum class eBindingType
   {
      FloatScalar,
      EulerAnglesRotation,
      Animation,
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
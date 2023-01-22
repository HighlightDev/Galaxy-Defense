#pragma once

#include <string>
#include <memory>

namespace EngineCore {

   enum class eBindingType
   {
      FloatScalar,
      EulerAnglesRotation,
      Animation,
      Boolean,
      Vec3
   };

   struct PropertyBinding
   {
   protected:

      bool bPropertyConnected = false;

   public:

      std::string BindingName;
      std::string EngineObjectName;
      std::string EngineObjectPropertyName;

      PropertyBinding(const std::string& bindingName)
         : BindingName(bindingName)
         , EngineObjectName("")
         , EngineObjectPropertyName("")
      {
      }

      virtual eBindingType GetBindingType() const = 0;
   };
}
#pragma once

#include <string>
#include <memory>

#include "Core/GameCore/EnginePropertyType.h"

namespace EngineCore {

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

      virtual eEnginePropertyBindingType GetBindingType() const = 0;

      bool IsPropertyConnected() const
      {
         return bPropertyConnected;
      }
   };
}
#pragma once

#include <string>

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"

namespace EngineCore {

   struct BooleanPropertyBinding
      : public PropertyBinding
   {
   private:
      
      std::shared_ptr<EngineObjectProperty<bool>> mGoProperty;

   public:

      BooleanPropertyBinding(const std::string& bindingName)
         : PropertyBinding(bindingName)
         , mGoProperty()
      {
      }

      void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<bool>>& gameObjectProperty)
      {
         assert(gameObjectProperty);
         mGoProperty = gameObjectProperty;
         bPropertyConnected = true;
      }

      void SetValue(const bool value) {
         assert(bPropertyConnected);
         mGoProperty->SetValue(value);
      }

      bool GetValue() const {
         assert(bPropertyConnected);
         return mGoProperty->GetValue();
      }

      eEnginePropertyBindingType GetBindingType() const override
      {
         return eEnginePropertyBindingType::Boolean;
      }
   };
}

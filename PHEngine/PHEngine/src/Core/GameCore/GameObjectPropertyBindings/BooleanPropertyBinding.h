#pragma once

#include <string>

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GameObjectProperty.h"

namespace EngineCore {

   struct BooleanPropertyBinding
      : public PropertyBinding
   {
   private:
      
      std::shared_ptr<EngineGOProperty<bool>> mGoProperty;

   public:

      BooleanPropertyBinding(const std::string& bindingName)
         : PropertyBinding(bindingName)
         , mGoProperty()
      {
      }

      void SetGameObjectProperty(const std::shared_ptr<EngineGOProperty<bool>>& gameObjectProperty)
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

      eBindingType GetBindingType() const override
      {
         return eBindingType::Boolean;
      }
   };
}

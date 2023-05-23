#pragma once

#include <string>

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"

namespace EngineCore {

   struct FloatPropertyBinding
      : public PropertyBinding
   {
   private:
      
      std::shared_ptr<EngineObjectProperty<float>> mGoProperty;

   public:

      FloatPropertyBinding(const std::string& bindingName)
         : PropertyBinding(bindingName)
         , mGoProperty()
      {
      }

      void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<float>> gameObjectProperty)
      {
         assert(gameObjectProperty);
         mGoProperty = gameObjectProperty;
         bPropertyConnected = true;
      }

      void SetValue(float value) {
         assert(bPropertyConnected);
         mGoProperty->SetValue(value);
      }

      float GetValue() const {
         assert(bPropertyConnected);
         return mGoProperty->GetValue();
      }

      eEnginePropertyBindingType GetBindingType() const override
      {
         return eEnginePropertyBindingType::FloatScalar;
      }
   };
}

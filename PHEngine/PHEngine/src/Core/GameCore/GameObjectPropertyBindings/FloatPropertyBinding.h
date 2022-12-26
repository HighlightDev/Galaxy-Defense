#pragma once

#include <string>

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GameObjectProperty.h"

namespace EngineCore {

   struct FloatPropertyBinding
      : public PropertyBinding
   {
   private:
      
      std::shared_ptr<EngineGOProperty<float>> mGoProperty;

   public:

      FloatPropertyBinding(const std::string& bindingName)
         : PropertyBinding(bindingName)
         , mGoProperty()
      {
      }

      void SetGameObjectProperty(const std::shared_ptr<EngineGOProperty<float>> gameObjectProperty)
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

      eBindingType GetBindingType() const override
      {
         return eBindingType::FloatScalar;
      }
   };
}

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
         bValueSet = true;
      }

      void SetValue(float value) {
         assert(bValueSet);
         mGoProperty->SetValue(value);
      }

      float GetValue() const {
         assert(bValueSet);
         return mGoProperty->GetValue();
      }

      virtual eBindingType GetBindingType() const override
      {
         return eBindingType::FloatScalar;
      }
   };
}

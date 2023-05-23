#pragma once

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{

   struct EulerAnglesRotationPropertyBinding
       : public PropertyBinding
   {
   private:
      std::shared_ptr<EngineObjectProperty<glm::vec3>> mGoProperty;

   public:
      EulerAnglesRotationPropertyBinding(const std::string &bindingName)
          : PropertyBinding(bindingName), mGoProperty()
      {
      }

      void SetEngineObjectProperty(const std::shared_ptr<EngineObjectProperty<glm::vec3>> &engineGoProperty)
      {
         assert(engineGoProperty);
         mGoProperty = engineGoProperty;
         bPropertyConnected = true;
      }

      void SetValue(const glm::vec3 &value)
      {
         assert(bPropertyConnected);
         mGoProperty->SetValue(value);
      }

      glm::vec3 GetValue() const
      {
         assert(bPropertyConnected);
         return mGoProperty->GetValue();
      }

      eEnginePropertyBindingType GetBindingType() const override
      {
         return eEnginePropertyBindingType::EulerAnglesRotation;
      }
   };
}

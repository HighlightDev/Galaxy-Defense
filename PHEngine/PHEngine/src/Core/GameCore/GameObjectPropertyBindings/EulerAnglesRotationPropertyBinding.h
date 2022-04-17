#pragma once

#include "PropertyBinding.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GameObjectProperty.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{

   struct EulerAnglesRotationPropertyBinding
       : public PropertyBinding
   {
   private:
      std::shared_ptr<EngineGOProperty<glm::vec3>> mGoProperty;

   public:
      EulerAnglesRotationPropertyBinding(const std::string &bindingName)
          : PropertyBinding(bindingName), mGoProperty()
      {
      }

      void SetGameObjectProperty(const std::shared_ptr<EngineGOProperty<glm::vec3>> &engineGoProperty)
      {
         assert(engineGoProperty);
         mGoProperty = engineGoProperty;
         bValueSet = true;
      }

      void SetValue(const glm::vec3 &value)
      {
         assert(bValueSet);
         mGoProperty->SetValue(value);
      }

      glm::vec3 GetValue() const
      {
         assert(bValueSet);
         return mGoProperty->GetValue();
      }

      virtual eBindingType GetBindingType() const override
      {
         return eBindingType::EulerAnglesRotation;
      }
   };
}

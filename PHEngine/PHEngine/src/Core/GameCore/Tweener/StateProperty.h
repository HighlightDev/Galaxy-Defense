#pragma once

#include <memory>
#include <string>

#include "Core/GameCore/GameObjectPropertyBindings/AnimationPropertyBinding.h"
#include "Core/GameCore/GameObjectPropertyBindings/FloatPropertyBinding.h"

namespace Game
{
   struct BaseStateProperty
   {
      std::weak_ptr<PropertyBinding> Binding;

      virtual eBindingType GetStatePropertyType() const = 0;

      BaseStateProperty(std::weak_ptr<PropertyBinding> propertyBinding)
         : Binding(propertyBinding)
      {
      }
   };

   template <eBindingType bindingType>
   struct StateProperty;

   template <>
   struct StateProperty<eBindingType::ANIMATION>
      : public BaseStateProperty
   {
      std::string AnimationName;

      virtual eBindingType GetStatePropertyType() const override
      {
         return eBindingType::ANIMATION;
      }

      StateProperty(const std::string& animationName, std::shared_ptr<AnimationPropertyBinding> animationPropertyBinding)
         : BaseStateProperty(animationPropertyBinding)
         , AnimationName(animationName)
      {
      }
   };

   template <>
   struct StateProperty<eBindingType::FLOAT>
      : public BaseStateProperty
   {
      float Value;

      virtual eBindingType GetStatePropertyType() const override
      {
         return eBindingType::FLOAT;
      }

      StateProperty(float value, std::shared_ptr<FloatPropertyBinding> animationPropertyBinding)
         : BaseStateProperty(animationPropertyBinding)
         , Value(value)
      {
      }
   };

}

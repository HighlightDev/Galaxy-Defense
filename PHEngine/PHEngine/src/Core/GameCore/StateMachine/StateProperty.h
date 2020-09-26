#pragma once

#include <memory>
#include <string>

#include "IStateMachineController.h"

namespace Game
{

   enum class StatePropertyType
   {
      Animation,
   };

   struct BaseStateProperty
   {
      std::weak_ptr<StatePropertyBinding> PropertyBinding;

      virtual StatePropertyType GetStatePropertyType() = 0;

      BaseStateProperty(std::weak_ptr<StatePropertyBinding> propertyBinding) 
         : PropertyBinding(propertyBinding)
      {
      }
   };

   template <StatePropertyType propType>
   struct StateProperty;

   template <>
   struct StateProperty<StatePropertyType::Animation>
      : public BaseStateProperty
   {
      std::string AnimationName;

      virtual StatePropertyType GetStatePropertyType() override
      {
         return StatePropertyType::Animation;
      }

      StateProperty(std::string animationName, std::shared_ptr<AnimationPropertyBinding> animationPropertyBinding)
         : BaseStateProperty(animationPropertyBinding)
         , AnimationName(animationName)
      {
      }
   };

}

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
      virtual StatePropertyType GetStatePropertyType() = 0;

      BaseStateProperty() { }
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

      StateProperty(std::string animationName)
         : BaseStateProperty()
         , AnimationName(animationName) { }
   };

}

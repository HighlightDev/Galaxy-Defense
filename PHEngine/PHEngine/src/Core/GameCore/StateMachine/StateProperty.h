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
      /* During transition this name will be used to determine which properties should be blended together */
      std::string PropertyName;

      std::weak_ptr<IStateMachineController> StateMachineControllerWP;

      virtual StatePropertyType GetStatePropertyType() = 0;

      BaseStateProperty(std::string propertyName, std::weak_ptr<IStateMachineController> stateMachineControllerWP)
         : PropertyName(propertyName)
         , StateMachineControllerWP(stateMachineControllerWP)
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

      StateProperty(std::string propertyName, std::weak_ptr<IStateMachineController> animationPlayerWP, std::string animationName)
         : BaseStateProperty(propertyName, animationPlayerWP)
         , AnimationName(animationName)
      {
      }
   };

}

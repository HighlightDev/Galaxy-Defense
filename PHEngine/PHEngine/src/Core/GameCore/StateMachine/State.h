#pragma once

#include <vector>
#include <memory>
#include <map>

#include "StateProperty.h"
#include "StateTransition.h"

namespace Game
{

   class State
   {
      std::string StateName;

      std::map<std::string /*Property Name*/, BaseStateProperty*> mStateProperties;

      std::map<std::string /*StateName*/, StateTransition> mTransitions;

   public:

      State()
         : mStateProperties()
      {
      }

      std::string GetStateName() const
      {
         return StateName;
      }

      void AddStateProperty(const std::string& propertyName, BaseStateProperty* stateProperty)
      {
         mStateProperties.emplace(std::make_pair(propertyName, stateProperty));
      }

      const std::map<std::string /*StateName*/, StateTransition>& GetTransitions() const
      {
         return mTransitions;
      }

      std::map<std::string /*Property Name*/, BaseStateProperty*> GetStateProperties() {
         return mStateProperties;
      }

   };

}

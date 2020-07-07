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

      std::vector<std::unique_ptr<BaseStateProperty>> mStateProperties;

      std::map<std::string /*StateName*/, StateTransition> mTransitions;

   public:

      State()
         : mStateProperties()
      {
      }

      void AddStateProperty(std::unique_ptr<BaseStateProperty> stateProperty)
      {
         mStateProperties.emplace_back(std::move(stateProperty));
      }

      const std::map<std::string /*StateName*/, StateTransition>& GetTransitions() const
      {
         return mTransitions;
      }

   };

}

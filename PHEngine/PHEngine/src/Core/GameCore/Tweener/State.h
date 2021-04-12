#pragma once

#include <vector>
#include <memory>
#include <map>

#include "StateProperty.h"
#include "StateTransition.h"
#include "Core/CommonCore/Assertion.h"

namespace Game
{

   class State
   {
      std::string mStateName;

      std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> mStateProperties;

      std::map<std::string /*dstStateName*/, StateTransition> mTransitions;

   public:

      State(const std::string& stateName)
         : mStateName(stateName)
         , mStateProperties()
      {
      }

      ~State()
      {
      }

      std::string GetStateName() const
      {
         return mStateName;
      }

      void AddStateProperty(BaseStateProperty* stateProperty)
      {
         auto bindingSP = stateProperty->PropertyBinding.lock();
         assert(bindingSP);
         const std::string& name = bindingSP->BindingName;
         assert(mStateProperties.count(name) == 0); // make sure that property doesn't duplicate
         mStateProperties.emplace(std::make_pair(name, stateProperty));
      }

      void AddStateTransition(const StateTransition& dstStateTransition)
      {
         if (auto spDestination = dstStateTransition.StateDestination.lock())
         {
            const std::string& dstStateName = spDestination->GetStateName();

            assert(mTransitions.count(dstStateName) == 0); // make sure that transition doesn't duplicate

            mTransitions.emplace(std::make_pair(dstStateName, dstStateTransition));
         }
      }

      const std::map<std::string /*dstStateName*/, StateTransition>& GetTransitions() const
      {
         return mTransitions;
      }

      std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> GetStateProperties() {
         return mStateProperties;
      }

   };

}

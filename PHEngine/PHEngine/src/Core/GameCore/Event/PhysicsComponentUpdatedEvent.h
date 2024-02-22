#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"

#include <string>

namespace Event
{
   class PhysicsComponentUpdatedGameThreadEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::string>>
   {
   public:
      using Event = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::string>>::Event_t;
      
      std::string ToString() const override {
         return "PhysicsComponentUpdatedGameThreadEvent";
      }
   };

}
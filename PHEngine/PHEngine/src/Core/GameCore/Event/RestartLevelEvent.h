#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"

#include <string>

namespace Event
{
   class RestartLevelGameThreadEvent
       : public TEvent<RestartLevelGameThreadEvent, eEventThreadType::GAME_THREAD, NoDataEventPolicy>
   {
   public:
      using Event = TEvent<RestartLevelGameThreadEvent, eEventThreadType::GAME_THREAD, NoDataEventPolicy>::Event_t;

      std::string ToString() const override
      {
         return "RestartLevelGameThreadEvent";
      }
   };

}
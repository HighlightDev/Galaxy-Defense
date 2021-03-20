#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"

#include <string>

namespace Event
{

   class PhysicsSimulationUpdatedEvent
      : public TEvent<SingleDataEventPolicy<std::string>>
   {
   public:
      using Event = TEvent<SingleDataEventPolicy<std::string>>::Event_t;
         
   };

}
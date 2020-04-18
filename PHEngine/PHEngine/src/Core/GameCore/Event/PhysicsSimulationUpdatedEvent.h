#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"

#include <string>

namespace Event
{

   class PhysicsSimulationUpdatedEvent
      : public TEvent<AtomicEventPolicy<std::string>>
   {
   public:
      using Event = TEvent<AtomicEventPolicy<std::string>>::Event_t;
         
   };

}
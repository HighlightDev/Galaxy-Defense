#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"

namespace Event
{

   class PlayerMovedEvent
      : public TEvent<AtomicEventPolicy<glm::vec3>>
   {
   public:
      using Event_t = TEvent<AtomicEventPolicy<glm::vec3>>::Event_t;
   };

}
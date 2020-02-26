#pragma once

#include "TEvent.h"

namespace Event
{

   class PlayerMovedEvent
      : public TEvent<glm::vec3>
   {
   public:
      using Event = TEvent<glm::vec3>::Event_t;
   };

}
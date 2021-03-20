#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event
{
   struct MouseMovedEvent
      : public TEvent<SingleDataEventPolicy<int>> // for now
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<int>>::Event_t;
   };

}

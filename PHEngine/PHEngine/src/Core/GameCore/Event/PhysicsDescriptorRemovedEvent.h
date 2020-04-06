#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event
{

   struct PhysicsDescriptorRemovedEvent
      : public TEvent<MultipleEventPolicy<size_t>>
   {
   public:
      using Event_t = TEvent<MultipleEventPolicy<size_t>>::Event_t;
   };

}
#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"

namespace Event
{

   class SceneComponentTransformChangedEvent
      : public TEvent<AtomicEventPolicy<uint64_t>>
   {
   public:
      using Event = TEvent<AtomicEventPolicy<uint64_t>>::Event_t;
         
   };

}
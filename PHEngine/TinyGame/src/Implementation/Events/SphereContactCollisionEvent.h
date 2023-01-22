#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Event/Policy/Policies.h"

#include <vector>
#include <stdint.h>
#include <cstdint>

namespace Event
{
   class SphereContactCollisionEvent
       : public TEvent<MultipleDataEventPolicy<std::vector<uint64_t>>>
   {
   public:
      using Event_t = TEvent<MultipleDataEventPolicy<std::vector<uint64_t>>>::Event_t;

      std::string ToString() const override
      {
         return "SphereContactCollisionEvent";
      }
   };

}
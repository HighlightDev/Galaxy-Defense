#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Event/Policy/Policies.h"

#include <vector>
#include <stdint.h>
#include <cstdint>

namespace Event
{
   class SphereContactCollisionEvent
       : public TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<uint64_t/*srcCollisionObject*/, std::vector<uint64_t>>/*dstCollisionObjects*/>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<uint64_t, std::vector<uint64_t>>>::Event_t;

      std::string ToString() const override
      {
         return "GameThreadSphereContactCollisionEvent";
      }
   };

}
#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Event/Policy/Policies.h"

#include <vector>
#include <stdint.h>
#include <cstdint>

namespace Event
{
   class ElectroRaySphereContactCollisionEvent
       : public TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<int32_t/*srcCollisionObject*/, std::vector<int32_t>>/*dstCollisionObjects*/>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<int32_t, std::vector<int32_t>>>::Event_t;

      std::string ToString() const override
      {
         return "GameThreadElectroRaySphereContactCollisionEvent";
      }
   };

}
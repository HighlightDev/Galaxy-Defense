#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event
{

   struct PhysicsDescriptorRemovedGameThreadEvent
      : public TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<size_t>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<size_t>>::Event_t;

      std::string ToString() const override {
         return "PhysicsDescriptorRemovedGameThreadEvent";
      }
   };

}
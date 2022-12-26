#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event
{

   struct PhysicsDescriptorRemovedEvent
      : public TEvent<MultipleDataEventPolicy<size_t>>
   {
   public:
      using Event_t = TEvent<MultipleDataEventPolicy<size_t>>::Event_t;

      std::string ToString() const override {
         return "PhysicsDescriptorRemovedEvent";
      }
   };

}
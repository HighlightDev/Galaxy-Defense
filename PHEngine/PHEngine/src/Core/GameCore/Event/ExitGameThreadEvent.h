#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event
{
   struct ExitGameThreadEvent
      : public TEvent<NoDataEventPolicy>
   {
   public:
      using Event_t = TEvent<NoDataEventPolicy>::Event_t;

      std::string ToString() const override {
         return "ExitGameThreadEvent";
      }
   };

}

#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event
{
   struct PauseGameThreadEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<bool>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<bool>>::Event_t;

      std::string ToString() const override {
         return "PauseGameThreadEvent";
      }
   };

}

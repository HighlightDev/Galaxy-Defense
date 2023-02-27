#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event
{
   struct ExitGameThreadEvent
      : public TEvent<eEventThreadType::GAME_THREAD, NoDataEventPolicy>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, NoDataEventPolicy>::Event_t;

      std::string ToString() const override {
         return "GameThreadExitGameThreadEvent";
      }
   };

}

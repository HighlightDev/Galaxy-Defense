#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

namespace Event
{
   struct MouseScrollEvent
      : public TEvent<SingleDataEventPolicy<Game::eMouseScrollDirection>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<Game::eMouseScrollDirection>>::Event_t;
   };

}

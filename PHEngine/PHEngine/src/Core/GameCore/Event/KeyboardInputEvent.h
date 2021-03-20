#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/Input/KeyboardData.h"

using namespace Game;

namespace Event
{
   struct KeyboardButtonDownEvent
      : public TEvent<SingleDataEventPolicy<KeyboardData>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<KeyboardData>>::Event_t;
   };

}

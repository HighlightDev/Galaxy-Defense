#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/Input/KeyboardData.h"

#include <vector>

using namespace Game;

namespace Event
{
   struct KeyboardButtonDownEvent
      : public TEvent<SingleDataEventPolicy<std::vector<KeyboardData>>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<std::vector<KeyboardData>>>::Event_t;
   };

}

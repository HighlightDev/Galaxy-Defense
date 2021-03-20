#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Components/Transform.h"

namespace Event
{

   class PlayerMovedEvent
      : public TEvent<SingleDataEventPolicy<std::weak_ptr<Game::Transform>>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<std::weak_ptr<Game::Transform>>>::Event_t;
   };

}
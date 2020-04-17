#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Components/Transform.h"

namespace Event
{

   class PlayerMovedEvent
      : public TEvent<AtomicEventPolicy<std::weak_ptr<Game::Transform>>>
   {
   public:
      using Event_t = TEvent<AtomicEventPolicy<std::weak_ptr<Game::Transform>>>::Event_t;
   };

}
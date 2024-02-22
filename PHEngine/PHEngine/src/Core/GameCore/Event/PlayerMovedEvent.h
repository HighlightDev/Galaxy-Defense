#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Components/Transform.h"

namespace Event
{

   class PlayerMovedGameThreadEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::weak_ptr<EngineCore::Transform>>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::weak_ptr<EngineCore::Transform>>>::Event_t;

      std::string ToString() const override {
         return "GameThreadPlayerMovedEvent";
      }
   };

}
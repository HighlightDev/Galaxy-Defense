#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Components/Transform.h"

namespace Event
{

   class PlayerMovedEvent
      : public TEvent<SingleDataEventPolicy<std::weak_ptr<EngineCore::Transform>>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<std::weak_ptr<EngineCore::Transform>>>::Event_t;

      virtual std::string ToString() const override {
         return "PlayerMovedEvent";
      }
   };

}
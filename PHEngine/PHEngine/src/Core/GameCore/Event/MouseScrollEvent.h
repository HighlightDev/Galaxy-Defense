#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

namespace Event
{
   struct MouseScrollEvent
      : public TEvent<SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>::Event_t;

      virtual std::string ToString() const override {
         return "MouseScrollEvent";
      }
   };

}

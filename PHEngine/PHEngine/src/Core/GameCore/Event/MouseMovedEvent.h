#pragma once

#include "Core/GameCore/Event/TEvent.h"

#include <glm/vec4.hpp>

namespace Event
{
   struct MouseMovedEvent
      : public TEvent<SingleDataEventPolicy<glm::ivec4/*X, Y, deltaX, deltaY*/>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<glm::ivec4>>::Event_t;

      virtual std::string ToString() const override {
         return "MouseMovedEvent";
      }
   };

}

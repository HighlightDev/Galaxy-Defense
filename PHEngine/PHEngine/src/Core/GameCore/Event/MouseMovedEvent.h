#pragma once

#include "Core/GameCore/Event/TEvent.h"

#include <glm/vec4.hpp>

namespace Event
{
   struct MouseMovedEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<glm::ivec4/*X, Y, deltaX, deltaY*/>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<glm::ivec4>>::Event_t;

      std::string ToString() const override {
         return "GameThreadMouseMovedEvent";
      }
   };

   struct LuaThreadMouseMovedEvent
      : public TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<glm::ivec4/*X, Y, deltaX, deltaY*/>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<glm::ivec4>>::Event_t;

      std::string ToString() const override {
         return "LuaThreadMouseMovedEvent";
      }
   };

}

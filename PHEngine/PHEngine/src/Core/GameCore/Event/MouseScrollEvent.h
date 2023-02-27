#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

namespace Event
{
   struct MouseScrollEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>::Event_t;

      std::string ToString() const override {
         return "GameThreadMouseScrollEvent";
      }
   };

   struct LuaThreadMouseScrollEvent
      : public TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>::Event_t;

      std::string ToString() const override {
         return "LuaThreadMouseScrollEvent";
      }
   };
}

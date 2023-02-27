#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"

#include <vector>

using namespace EngineCore;

namespace Event
{
   struct MouseButtonDownEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::vector<MouseKeysData>>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::vector<MouseKeysData>>>::Event_t;

      std::string ToString() const override {
         return "GameThreadMouseButtonDownEvent";
      }
   };

   struct LuaThreadMouseButtonDownEvent
      : public TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<MouseKeysData>>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<MouseKeysData>>>::Event_t;

      std::string ToString() const override {
         return "LuaThreadMouseButtonDownEvent";
      }
   };
}

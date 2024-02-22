#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"

#include <vector>

using namespace EngineCore;

namespace Event
{
   struct KeyboardButtonDownGameThreadEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::vector<KeyboardKeysData>>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::vector<KeyboardKeysData>>>::Event_t;

      std::string ToString() const override {
         return "KeyboardButtonDownGameThreadEvent";
      }
   };

   struct KeyboardButtonDownLuaThreadEvent
      : public TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<KeyboardKeysData>>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<KeyboardKeysData>>>::Event_t;

      std::string ToString() const override {
         return "KeyboardButtonDownLuaThreadEvent";
      }
   };
}

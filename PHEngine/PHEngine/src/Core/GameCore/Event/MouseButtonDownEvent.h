#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

#include <glm/vec2.hpp>
#include <vector>

using namespace EngineCore;

namespace Event
{
   struct MouseButtonDownRootEvent
       : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<glm::ivec2, std::vector<MouseKeysData>>>
   {
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<glm::ivec2, std::vector<MouseKeysData>>>::Event_t;

      std::string ToString() const override
      {
         return "MouseButtonDownRootEvent";
      }
   };

   struct MouseButtonDownGameThreadEvent
       : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eMouseEventTargetReceiverType, std::vector<MouseKeysData>>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eMouseEventTargetReceiverType, std::vector<MouseKeysData>>>::Event_t;

      std::string ToString() const override
      {
         return "MouseButtonDownGameThreadEvent";
      }
   };

   struct MouseButtonDownLuaThreadEvent
       : public TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<MouseKeysData>>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<MouseKeysData>>>::Event_t;

      std::string ToString() const override
      {
         return "MouseButtonDownLuaThreadEvent";
      }
   };
}

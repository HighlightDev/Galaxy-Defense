#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Event/Policy/Policies.h"
#include "Implementation/StatusTypes.h"

using namespace Game;

namespace Event
{
   class LevelProgressChangedEvent
       : public TEvent<LevelProgressChangedEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eLevelProgressStatusType, std::string/*args*/>>
   {
   public:
      using Event_t = TEvent<LevelProgressChangedEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eLevelProgressStatusType, std::string/*args*/>>::Event_t;

      std::string ToString() const override
      {
         return "GameThreadLevelProgressChangedEvent";
      }
   };

   class LuaLevelProgressChangedEvent
       : public TEvent<LuaLevelProgressChangedEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eLevelProgressStatusType, std::string/*args*/>>
   {
   public:
      using Event_t = TEvent<LuaLevelProgressChangedEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eLevelProgressStatusType, std::string/*args*/>>::Event_t;

      std::string ToString() const override
      {
         return "LuaThreadLevelProgressChangedEvent";
      }
   };

}
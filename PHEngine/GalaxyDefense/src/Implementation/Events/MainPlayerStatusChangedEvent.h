#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Event/Policy/Policies.h"
#include "Implementation/StatusTypes.h"

using namespace Game;

namespace Event
{
   class MainPlayerStatusChangedEvent
       : public TEvent<MainPlayerStatusChangedEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType, std::string/*args*/>>
   {
   public:
      using Event_t = TEvent<MainPlayerStatusChangedEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType, std::string/*args*/>>::Event_t;

      std::string ToString() const override
      {
         return "GameThreadMainPlayerStatusChangedEvent";
      }
   };

   class LuaMainPlayerStatusChangedEvent
       : public TEvent<LuaMainPlayerStatusChangedEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType, std::string/*args*/>>
   {
   public:
      using Event_t = TEvent<LuaMainPlayerStatusChangedEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType, std::string/*args*/>>::Event_t;

      std::string ToString() const override
      {
         return "LuaThreadMainPlayerStatusChangedEvent";
      }
   };

}
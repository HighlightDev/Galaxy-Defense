#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Event/Policy/Policies.h"
#include "Implementation/StatusTypes.h"

using namespace Game;

namespace Event
{
   class MainPlayerStatusChangedEvent
       : public TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType>>::Event_t;

      std::string ToString() const override
      {
         return "GameThreadMainPlayerStatusChangedEvent";
      }
   };

   class LuaMainPlayerStatusChangedEvent
       : public TEvent<eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType>>::Event_t;

      std::string ToString() const override
      {
         return "LuaThreadMainPlayerStatusChangedEvent";
      }
   };

}
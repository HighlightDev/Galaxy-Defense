#pragma once

#include <type_traits>

namespace Event
{
   template <typename... EventTypes>
   struct LuaThreadEventIterator;

   template <>
   struct LuaThreadEventIterator<>
   {
      static void IterateRegisterEvent()
      {
      }
   };

   struct LuaThreadEventDispatcher
   {
   private:
      std::vector<IEvent *> m_eventInstances;

   public:
      static LuaThreadEventDispatcher *GetInstance()
      {
         static LuaThreadEventDispatcher m_instance = LuaThreadEventDispatcher();
         return &m_instance;
      }

      template <typename EventType>
      typename std::enable_if<std::is_base_of<IEvent, EventType>::value, void>::type RegisterEventByType()
      {
         m_eventInstances.emplace_back(EventType::GetInstance());
      }

      template <typename... EventTypes>
      void RegisterEventsByType()
      {
         LuaThreadEventIterator<EventTypes...>::IterateRegisterEvent();
      }

      void UnregisterEvents()
      {
         m_eventInstances.clear();
      }

      void ProcessEvents(eExecutionOrder order)
      {
         for (const auto &eventInstance : m_eventInstances)
         {
            eventInstance->ProcessCachedEvents(order);
         }
      }

   private:
      LuaThreadEventDispatcher()
          : m_eventInstances()
      {
      }
   };

   template <typename EventType, typename... EventTypes>
   struct LuaThreadEventIterator<EventType, EventTypes...>
   {
      static void IterateRegisterEvent()
      {
         LuaThreadEventDispatcher::GetInstance()->RegisterEventByType<EventType>();
         LuaThreadEventIterator<EventTypes...>::IterateRegisterEvent();
      }
   };
}

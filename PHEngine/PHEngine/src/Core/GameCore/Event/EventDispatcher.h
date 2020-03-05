#pragma once

#include "PlayerMovedEvent.h"
#include "CameraTransformChangedEvent.h"
#include "SceneComponentTransformChangedEvent.h"

#include <tuple>

namespace Event {

   template <typename ParentEventDispatcher, size_t eventIndex>
   struct EventsIterator
   {
      static void IterateParentEventDispatcher()
      {
         using event_t = typename std::tuple_element<eventIndex, typename ParentEventDispatcher::EventTypes_t>::type;
         event_t::GetInstance()->ProcessCachedEvents();
        
         EventsIterator<ParentEventDispatcher, eventIndex - 1>::IterateParentEventDispatcher();
      }
   };

   template <typename ParentEventDispatcher>
   struct EventsIterator<ParentEventDispatcher, -1>
   {
      static void IterateParentEventDispatcher() { }
   };

   template <typename... EventTypes>
   struct EventDispatcher
   {
      using EventTypes_t = std::tuple<EventTypes...>;

      static constexpr size_t registeredEventsCount = std::tuple_size<std::tuple<EventTypes...>>::value;

      static void ProcessEvents() {

         EventsIterator<EventDispatcher<EventTypes...>, registeredEventsCount - 1>::IterateParentEventDispatcher();
      }
   };
   
   using EngineEventDispatcher = EventDispatcher<
      CameraTransformChangedEvent,
      PlayerMovedEvent,
      SceneComponentTransformChangedEvent
   >;

}


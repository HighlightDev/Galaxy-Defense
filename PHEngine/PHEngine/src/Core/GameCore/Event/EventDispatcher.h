#pragma once

#include "PlayerMovedEvent.h"
#include "CameraTransformChangedEvent.h"
#include "PhysicsSimulationUpdatedEvent.h"
#include "KeyboradInputEvent.h"
#include "KinematicBodyMovedEvent.h"

#include <tuple>

namespace Event {

   namespace EventDispatcherCore
   {
      template <typename ParentEventDispatcher, size_t eventIndex>
      struct EventsIterator
      {
         static void IterateParentEventDispatcher(ExecutionOrder order)
         {
            using event_t = typename std::tuple_element<eventIndex, typename ParentEventDispatcher::EventTypes_t>::type;
            event_t::GetInstance()->ProcessCachedEvents(order);

            EventsIterator<ParentEventDispatcher, eventIndex - 1>::IterateParentEventDispatcher(order);
         }
      };

      template <typename ParentEventDispatcher>
      struct EventsIterator<ParentEventDispatcher, -1>
      {
         static void IterateParentEventDispatcher(ExecutionOrder order) { }
      };
   }

   template <typename... EventTypes>
   struct EventDispatcher
   {
      using EventTypes_t = std::tuple<EventTypes...>;

      static constexpr size_t registeredEventsCount = std::tuple_size<std::tuple<EventTypes...>>::value;

      static void ProcessEvents(ExecutionOrder order) {

         EventDispatcherCore::EventsIterator<EventDispatcher<EventTypes...>, registeredEventsCount - 1>::IterateParentEventDispatcher(order);
      }
   };
   
   using EngineEventDispatcher = EventDispatcher<
      CameraTransformChangedEvent,
      PlayerMovedEvent,
      PhysicsSimulationUpdatedEvent,
      KeyboardButtonDownEvent,
      KinematicBodyMovedEvent
   >;

}


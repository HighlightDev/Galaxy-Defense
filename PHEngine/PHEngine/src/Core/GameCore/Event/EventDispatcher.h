#pragma once

#include "PlayerMovedEvent.h"
#include "CameraTransformChangedEvent.h"
#include "PhysicsComponentUpdatedEvent.h"
#include "KeyboardInputEvent.h"
#include "KinematicBodyMovedEvent.h"
#include "TextureAtlasGeneratedEvent.h"
#include "MouseMovedEvent.h"
#include "MouseScrollEvent.h"
#include "MouseButtonDownEvent.h"
#include "PhysicsCollisionOccuredEvent.h"
#include "TextEvent.h"

#include <type_traits>

namespace Event
{

   template <typename... EventTypes>
   struct EventIterator;


   template <>
   struct EventIterator<>
   {
      static void IterateRegisterEvent()
      {
      }
   };

   struct EventDispatcher
   {
   private:
      std::vector<IEvent *> m_eventInstances;

   public:
      static EventDispatcher *GetInstance()
      {
         static EventDispatcher m_instance = EventDispatcher();
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
         EventIterator<EventTypes...>::IterateRegisterEvent();
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
      EventDispatcher()
          : m_eventInstances()
      {
      }
   };

   template <typename EventType, typename... EventTypes>
   struct EventIterator<EventType, EventTypes...>
   {
      static void IterateRegisterEvent()
      {
         EventDispatcher::GetInstance()->RegisterEventByType<EventType>();
         EventIterator<EventTypes...>::IterateRegisterEvent();
      }
   };
}

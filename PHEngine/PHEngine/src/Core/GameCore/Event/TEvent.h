#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <tuple>

#include "Policy/Policies.h"

namespace Event
{
   template <typename EventHandlePolicy>
   class TEvent
   {
   public:

      using EventHandlePolicy_t = EventHandlePolicy;
      using Event_t = TEvent<EventHandlePolicy>;
      using EventData_t = typename EventHandlePolicy_t::TupleData_t;

   private:

      static Event_t* m_instance;

      typename EventHandlePolicy mPolicy;

      std::vector<TEvent<EventHandlePolicy_t>*> m_listeners;

   protected:

      TEvent() {
      }

   public:

      virtual ~TEvent() {
      }

      static Event_t* GetInstance() {

         if (!m_instance)
            m_instance = new Event_t();

         return m_instance;
      }

      virtual void ProcessEvent(const EventData_t& data) { }

      void AddListener(Event_t* eventListener)
      {
         m_listeners.push_back(eventListener);
      }

      void RemoveListener(Event_t* eventListener)
      {
         auto it = std::find(m_listeners.begin(), m_listeners.end(), eventListener);
         if (it != m_listeners.end())
            m_listeners.erase(it);
      }

      template <typename... DataTypesT>
      void SendEvent(DataTypesT&&... data)
      {
         mPolicy.EmplaceData(std::forward<DataTypesT>(data)...);
      }

      void ProcessCachedEvents()
      {
         while (mPolicy.HasData())
         {
            EventData_t packedData = mPolicy.PopData();
            for (auto& listener : m_listeners)
            {
               listener->ProcessEvent(packedData);
            }
         }
      }
   };
}


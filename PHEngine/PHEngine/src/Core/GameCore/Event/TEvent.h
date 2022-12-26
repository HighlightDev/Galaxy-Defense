#pragma once

#include <vector>
#include <tuple>
#include <algorithm>

#include "IEvent.h"
#include "Policy/Policies.h"

namespace Event
{

   template <typename EventHandlePolicy>
   class TEvent : public IEvent
   {
   public:
      using EventHandlePolicy_t = EventHandlePolicy;
      using Event_t = TEvent<EventHandlePolicy>;
      using EventData_t = typename EventHandlePolicy_t::TupleData_t;

   private:
      EventHandlePolicy mPolicy[2];

      std::vector<TEvent<EventHandlePolicy_t> *> m_listeners;

   protected:
      TEvent()
      {
      }

   public:
      virtual ~TEvent()
      {
      }

      static Event_t *GetInstance()
      {
         static Event_t m_instance;
         return &m_instance;
      }

      std::string ToString() const override
      {
         return "TEvent";
      }

      template <typename... DataTypesT>
      void SendEvent(const eExecutionOrder order, DataTypesT &&...data)
      {
         mPolicy[(int32_t)order].EmplaceData(std::forward<DataTypesT>(data)...);
      }

      void ProcessCachedEvents(const eExecutionOrder currentOrder) override
      {
         while (mPolicy[currentOrder].HasData())
         {
            const EventData_t &packedData = mPolicy[currentOrder].PopData();
            for (auto &listener : m_listeners)
            {
               listener->ProcessEvent(packedData);
            }
         }
      }

      void AddListener(Event_t *eventListener)
      {
         m_listeners.push_back(eventListener);
      }

      void RemoveListener(Event_t *eventListener)
      {
         auto it = std::find(m_listeners.begin(), m_listeners.end(), eventListener);
         if (it != m_listeners.end())
            m_listeners.erase(it);
      }

   protected:
      virtual void ProcessEvent(const EventData_t &data) {}
   };
}

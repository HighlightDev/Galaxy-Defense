#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <tuple>

namespace Event
{
   template <typename... DataTypes>
   class TEvent
   {
   public:

      using Event_t = TEvent<DataTypes...>;
      using EventData_t = std::tuple<DataTypes...>;

   private:

      static Event_t* m_instance;

      std::vector<TEvent<DataTypes...>*> m_listeners;

   protected:

      TEvent();

   public:

      virtual ~TEvent();

      void AddListener(Event_t* eventListener);

      void RemoveListener(Event_t* eventListener);

      virtual void ProcessEvent(const EventData_t& data);

      static Event_t* GetInstance() {

         if (!m_instance)
            m_instance = new Event_t();

         return m_instance;
      }

      template <typename... DataTypesT>
      void SendEvent(DataTypesT&&... data) const
      {
         EventData_t packedData = std::make_tuple(std::forward<DataTypesT>(data)...);
         for (auto& listener : m_listeners)
         {
            listener->ProcessEvent(packedData);
         }
      }

   };
}


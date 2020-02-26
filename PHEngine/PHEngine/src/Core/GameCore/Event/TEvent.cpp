#include "TEvent.h"

#include <utility>

namespace Event
{

   template class TEvent<glm::vec3>;
   template class TEvent<uint64_t>;

   template <typename... DataTypes>
   TEvent<DataTypes...>* TEvent<DataTypes...>::m_instance = nullptr;

   template <typename... DataTypes>
   TEvent<DataTypes...>::TEvent()
   {
   }

   template <typename... DataTypes>
   TEvent<DataTypes...>::~TEvent()
   {
   }

   template <typename... DataTypes>
   void TEvent<DataTypes...>::ProcessEvent(const EventData_t& data)
   {
   }

   template <typename... DataTypes>
   void TEvent<DataTypes...>::AddListener(Event_t* eventListener)
   {
      m_listeners.push_back(eventListener);
   }

   template <typename... DataTypes>
   void TEvent<DataTypes...>::RemoveListener(Event_t* eventListener)
   {
      auto it = std::find(m_listeners.begin(), m_listeners.end(), eventListener);
      if (it != m_listeners.end())
         m_listeners.erase(it);
   }

}
#pragma once

#include <type_traits>

namespace Event {

template<typename... EventTypes>
struct GameThreadEventIterator;

template<>
struct GameThreadEventIterator<> {
    static void IterateRegisterEvent()
    {
    }
};

struct GameThreadEventDispatcher {
private:
    std::vector<IEvent*> m_eventInstances;

public:
    static GameThreadEventDispatcher* GetInstance()
    {
        static GameThreadEventDispatcher m_instance = GameThreadEventDispatcher();
        return &m_instance;
    }

    template<typename EventType>
    typename std::enable_if<std::is_base_of<IEvent, EventType>::value, void>::type RegisterEventByType()
    {
        m_eventInstances.emplace_back(EventType::GetInstance());
    }

    template<typename... EventTypes>
    void RegisterEventsByType()
    {
        GameThreadEventIterator<EventTypes...>::IterateRegisterEvent();
    }

    void UnregisterEvents()
    {
        m_eventInstances.clear();
    }

    void ProcessEvents(eExecutionOrder order)
    {
        for (const auto& eventInstance : m_eventInstances) {
            eventInstance->ProcessCachedEvents(order);
        }
    }

private:
    GameThreadEventDispatcher()
        : m_eventInstances()
    {
    }
};

template<typename EventType, typename... EventTypes>
struct GameThreadEventIterator<EventType, EventTypes...> {
    static void IterateRegisterEvent()
    {
        GameThreadEventDispatcher::GetInstance()->RegisterEventByType<EventType>();
        GameThreadEventIterator<EventTypes...>::IterateRegisterEvent();
    }
};
} // namespace Event

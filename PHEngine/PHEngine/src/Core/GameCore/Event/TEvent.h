#pragma once

#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"
#include "IEvent.h"
#include "Policy/Policies.h"

#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>
#include <tuple>
#include <vector>

using namespace EngineCore;

namespace Event {
template<typename DerivedEventType, eEventThreadType threadType, typename EventHandlePolicy>
class TEvent : public IEvent {
public:
    using EventHandlePolicy_t = EventHandlePolicy;
    using Event_t = TEvent<DerivedEventType, threadType, EventHandlePolicy>;
    using EventData_t = typename EventHandlePolicy_t::TupleData_t;
    using DerivedEventType_t = DerivedEventType;

private:
    static constexpr eEventThreadType mThreadType = threadType;

    std::mutex mListenersMutex;

    EventHandlePolicy mPolicy[2];

    std::vector<std::weak_ptr<Event_t>> m_listeners;

protected:
    TEvent()
        : IEvent()
    {
    }

public:
    virtual ~TEvent()
    {
    }

    static Event_t* GetInstance()
    {
        static Event_t m_instance;
        return &m_instance;
    }

    std::string ToString() const override
    {
        return "TEvent";
    }

    template<typename... DataTypesT>
    void SendEvent(const eExecutionOrder order, DataTypesT&&... data)
    {
        mPolicy[(int32_t)order].EmplaceData(std::forward<DataTypesT>(data)...);
    }

    void ProcessCachedEvents(const eExecutionOrder currentOrder) override
    {
        while (mPolicy[currentOrder].HasData()) {
            const EventData_t& packedData = mPolicy[currentOrder].PopData();

            for (const auto& listenerWp : m_listeners) {
                if (const auto& listenerSp = listenerWp.lock()) {
                    listenerSp->ProcessEvent(static_cast<DerivedEventType_t*>(this), packedData);
                }
            }
        }
    }

    void AddListener(const std::shared_ptr<Event_t>& eventListener)
    {
        std::lock_guard<std::mutex> lockEmplace(mListenersMutex);
        m_listeners.emplace_back(eventListener);
    }

    void RemoveListener(const size_t instanceId)
    {
        std::lock_guard<std::mutex> lockRemove(mListenersMutex);
        if (m_listeners.size()) {
            m_listeners.erase(
                std::remove_if(
                    m_listeners.begin(),
                    m_listeners.end(),
                    [instanceId](const auto& listenerWp) {
                        if (const auto& listenerSp = listenerWp.lock()) {
                            return listenerSp->GetInstanceId() == instanceId;
                        }

                        return true;
                    }),
                m_listeners.end());
        }
    }

protected:
    virtual void ProcessEvent(const DerivedEventType_t* senderPtr, const EventData_t& data)
    {
    }
};
} // namespace Event

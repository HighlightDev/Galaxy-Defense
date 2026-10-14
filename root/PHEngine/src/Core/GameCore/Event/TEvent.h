#pragma once

#include "Core/CommonCore/Assertion.h"
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
    using Type_t = TEvent<DerivedEventType, threadType, EventHandlePolicy>;
    using DerivedEventType_t = DerivedEventType;
    using EventHandlePolicy_t = EventHandlePolicy;
    using EventData_t = typename EventHandlePolicy_t::TupleData_t;

private:
    static constexpr eEventThreadType mThreadType = threadType;
    static constexpr const char* mThreadName = eEventThreadType::GAME_THREAD == mThreadType
        ? "GameThread"
        : eEventThreadType::LUA_THREAD == mThreadType ? "LuaThread" : "UnknownThread";

    std::mutex mListenersMutex;

    EventHandlePolicy mPolicy[2];

    std::vector<std::weak_ptr<Type_t>> m_listeners;

    bool bLogEvent{false};

protected:
    TEvent(const bool _bLogEvent = false)
        : IEvent()
        , mPolicy()
        , bLogEvent(_bLogEvent)
    {
    }

public:
    virtual ~TEvent()
    {
    }

    static DerivedEventType* GetInstance()
    {
        static DerivedEventType* m_instance = new DerivedEventType();
        return m_instance;
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
            const EventData_t packedData = mPolicy[currentOrder].PopData();
            if (bLogEvent) {
                LogInfo(ToString(), "::ProcessCachedEvents: ", packedData);
            }

            for (const auto& listenerWp : m_listeners) {
                if (const auto& listenerSp = listenerWp.lock()) {
                    listenerSp->ProcessEvent(reinterpret_cast<DerivedEventType_t*>(this), packedData);
                }
            }
        }
    }

    void AddListener(const std::shared_ptr<Type_t>& eventListener)
    {
        if (bLogEvent) {
            LogInfo(ToString(), "::AddListener");
        }
        std::lock_guard<std::mutex> lockEmplace(mListenersMutex);
        m_listeners.emplace_back(eventListener);
    }

    void RemoveListener(const size_t instanceId)
    {
        if (bLogEvent) {
            LogInfo(ToString(), "::RemoveListener: instanceId ", instanceId);
        }
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

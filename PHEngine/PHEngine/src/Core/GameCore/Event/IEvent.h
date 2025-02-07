#pragma once

#include <string>

namespace Event {
enum eExecutionOrder { PRE_EXECUTION = 0, POST_EXECUTION = 1 };

enum eEventThreadType { GAME_THREAD, LUA_THREAD };

class IEvent {
    static size_t s_instanceId;

    size_t m_instanceId;

public:
    IEvent();

    virtual void ProcessCachedEvents(const eExecutionOrder currentOrder) = 0;

    virtual std::string ToString() const = 0;

    size_t GetInstanceId() const;
};
} // namespace Event
#pragma once

#include <string>

namespace Event
{
    enum eExecutionOrder
    {
        PRE_EXECUTION = 0,
        POST_EXECUTION = 1
    };

    enum eEventThreadType
    {
        GAME_THREAD,
        LUA_THREAD
    };

    class IEvent
    {
    public:
        virtual void ProcessCachedEvents(const eExecutionOrder currentOrder) = 0;

        virtual std::string ToString() const = 0;
    };
}
#pragma once

#include <string>

namespace Event
{
    enum eExecutionOrder
    {
        PRE_EXECUTION = 0,
        POST_EXECUTION = 1
    };

    class IEvent
    {
    public:
        virtual void ProcessCachedEvents(const eExecutionOrder currentOrder) = 0;

        virtual std::string ToString() const = 0;
    };
}
#pragma once

namespace Event
{
    enum ExecutionOrder
    {
        PRE_EXECUTION = 0,
        POST_EXECUTION = 1
    };

    class IEvent
    {
    public:
        virtual void ProcessCachedEvents(const ExecutionOrder currentOrder) = 0;
    };
}
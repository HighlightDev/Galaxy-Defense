#pragma once

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
    };
}
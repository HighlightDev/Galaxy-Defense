#include "MouseBindings.h"

    #include <iostream>

namespace Game
{

    MouseBindings::MouseBindings()
        : mLastMouseMoveEvent(0), bMouseMoveEventDirty(false)
    {
        MouseMovedEvent::GetInstance()->AddListener(this);
    }

    MouseBindings::~MouseBindings()
    {
        MouseMovedEvent::GetInstance()->RemoveListener(this);
    }

    void MouseBindings::ProcessEvent(const typename MouseMovedEvent::EventData_t &mouseData)
    {
        const glm::ivec4 &mouseMoveData = std::get<0>(mouseData);
        PushMouseMoveEvent(mouseMoveData);
    }

    bool MouseBindings::IsMouseMoveEventDirty() const
    {
        return bMouseMoveEventDirty;
    }

    glm::ivec4 MouseBindings::FlushMouseMoveEvent()
    {
        glm::ivec4 result = glm::ivec4(0);

        if (bMouseMoveEventDirty)
        {
            result = mLastMouseMoveEvent;
            bMouseMoveEventDirty = false;
        }

        return result;
    }

    void MouseBindings::PushMouseMoveEvent(const glm::ivec4 &moveEvent)
    {
        mLastMouseMoveEvent = moveEvent;
        bMouseMoveEventDirty = true;
    }
}
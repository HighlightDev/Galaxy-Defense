#include "MouseBindings.h"

#include <iostream>

namespace EngineCore
{

    MouseBindings::MouseBindings()
        : mLastMouseMoveEvent(0), bMouseMoveEventDirty(false), mLastMouseScrollDirectionEvent(eMouseScrollDirection::Undefined), bMouseScrollEventDirty(false)
    {
        MouseMovedEvent::GetInstance()->AddListener(this);
        MouseScrollEvent::GetInstance()->AddListener(this);
    }

    MouseBindings::~MouseBindings()
    {
        MouseMovedEvent::GetInstance()->RemoveListener(this);
        MouseScrollEvent::GetInstance()->RemoveListener(this);
    }

    void MouseBindings::ProcessEvent(const typename MouseMovedEvent::EventData_t &mouseData)
    {
        const glm::ivec4 &mouseMoveData = std::get<0>(mouseData);
        PushMouseMoveEvent(mouseMoveData);
    }

    void MouseBindings::ProcessEvent(const typename MouseScrollEvent::EventData_t &mouseData)
    {
        const eMouseScrollDirection mouseScrollDirection = std::get<0>(mouseData);
        PushMouseScrollEvent(mouseScrollDirection);
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

    bool MouseBindings::IsMouseScrollEventDirty() const
    {
        return bMouseScrollEventDirty;
    }

    eMouseScrollDirection MouseBindings::FlushMouseScrollEvent()
    {
        eMouseScrollDirection result = eMouseScrollDirection::Undefined;

        if (bMouseScrollEventDirty)
        {
            result = mLastMouseScrollDirectionEvent;
            bMouseScrollEventDirty = false;
        }

        return result;
    }

    void MouseBindings::PushMouseScrollEvent(const eMouseScrollDirection mouseScrollEvent)
    {
        mLastMouseScrollDirectionEvent = mouseScrollEvent;
        bMouseScrollEventDirty = true;
    }
}
#include "MouseBindings.h"

#include <iostream>

namespace EngineCore
{

    MouseBindings::MouseBindings()
        : mLastMouseMoveEvent(0),
          bMouseMoveEventDirty(false),
          mLastMouseScrollDirectionEvent(eMouseScrollDirection::Undefined),
          bMouseScrollEventDirty(false),
          mMouseKeysMaskVec(),
          mPressedMouseKeysCount(0)
    {
        mMouseKeysMaskVec.reserve(3);
    }

    MouseBindings::~MouseBindings()
    {
        UnsubscribeFromEvents();
    }

    void MouseBindings::UnsubscribeFromEvents()
    {
        MouseMovedEvent::GetInstance()->RemoveListener(MouseMovedEvent::GetInstanceId());
        MouseScrollEvent::GetInstance()->RemoveListener(MouseScrollEvent::GetInstanceId());
        MouseButtonDownEvent::GetInstance()->RemoveListener(MouseButtonDownEvent::GetInstanceId());
    }

    void MouseBindings::SubscribeOnEvents()
    {
        MouseMovedEvent::GetInstance()->AddListener(shared_from_this());
        MouseScrollEvent::GetInstance()->AddListener(shared_from_this());
        MouseButtonDownEvent::GetInstance()->AddListener(shared_from_this());
    }

    void MouseBindings::Initialize()
    {
        SubscribeOnEvents();
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

    void MouseBindings::ProcessEvent(const typename MouseButtonDownEvent::EventData_t &data)
    {
        mMouseKeysMaskVec = std::move(std::get<0>(data));
    }

    KeyState MouseBindings::GetKeyState(const eMouseKeys mouseButtonKey) const
    {
        KeyState state = KeyState::RELEASED;

        auto it = std::find_if(mMouseKeysMaskVec.begin(),
                               mMouseKeysMaskVec.end(), [=](const auto &keyData) -> bool
                               { return keyData.Key == mouseButtonKey; });

        if (it != mMouseKeysMaskVec.end())
        {
            state = it->State;
        }

        return state;
    }

    void MouseBindings::SetIsReceivingMouseEvents(const bool receiveMouseEvents)
    {
        if (bReceiveMouseEvents != receiveMouseEvents)
        {
            bReceiveMouseEvents = receiveMouseEvents;
            if (bReceiveMouseEvents)
            {
                SubscribeOnEvents();
            }
            else
            {
                UnsubscribeFromEvents();
                ClearMouseMoveCache();
                ClearMouseScrollCache();
            }
        }
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

    glm::ivec4 MouseBindings::GetLastMouseCursorPosition() const
    {
        return mLastMouseMoveEvent;
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

    void MouseBindings::ClearMouseScrollCache()
    {
        mLastMouseScrollDirectionEvent = eMouseScrollDirection::Undefined;
        bMouseScrollEventDirty = false;
    }

    void MouseBindings::ClearMouseMoveCache()
    {
        mLastMouseMoveEvent = {};
        bMouseMoveEventDirty = false;
    }
}
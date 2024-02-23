#include "UiMouseBindings.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

#include <iostream>

namespace EngineCore
{

    UiMouseBindings::UiMouseBindings()
        : mLastMouseMoveEvent(0),
          bMouseMoveEventDirty(false),
          mLastMouseScrollDirectionEvent(eMouseScrollDirection::Undefined),
          bMouseScrollEventDirty(false),
          mMouseKeysMaskVec(),
          mPressedMouseKeysCount(0)
    {
        mMouseKeysMaskVec.reserve(3);
    }

    UiMouseBindings::~UiMouseBindings()
    {
        UnsubscribeFromEvents();
    }

    void UiMouseBindings::UnsubscribeFromEvents()
    {
        MouseMovedGameThreadEvent::GetInstance()->RemoveListener(MouseMovedGameThreadEvent::GetInstanceId());
        MouseScrollGameThreadEvent::GetInstance()->RemoveListener(MouseScrollGameThreadEvent::GetInstanceId());
        MouseButtonDownGameThreadEvent::GetInstance()->RemoveListener(MouseButtonDownGameThreadEvent::GetInstanceId());
    }

    void UiMouseBindings::SubscribeOnEvents()
    {
        MouseMovedGameThreadEvent::GetInstance()->AddListener(shared_from_this());
        MouseScrollGameThreadEvent::GetInstance()->AddListener(shared_from_this());
        MouseButtonDownGameThreadEvent::GetInstance()->AddListener(shared_from_this());
    }

    void UiMouseBindings::Initialize()
    {
        SubscribeOnEvents();
    }

    void UiMouseBindings::ProcessEvent(const typename MouseMovedGameThreadEvent::EventData_t &mouseData)
    {
        const glm::ivec4 &mouseMoveData = std::get<0>(mouseData);
        PushMouseMoveEvent(mouseMoveData);
    }

    void UiMouseBindings::ProcessEvent(const typename MouseScrollGameThreadEvent::EventData_t &mouseData)
    {
        const eMouseScrollDirection mouseScrollDirection = std::get<0>(mouseData);
        PushMouseScrollEvent(mouseScrollDirection);
    }

    void UiMouseBindings::ProcessEvent(const typename MouseButtonDownGameThreadEvent::EventData_t &data)
    {
        if (eMouseEventTargetReceiverType::UI_INPUT_SYSTEM == std::get<0>(data))
        {
            mMouseKeysMaskVec = std::get<1>(data);
        }
    }

    KeyState UiMouseBindings::GetKeyState(const eMouseKeys mouseButtonKey) const
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

    void UiMouseBindings::SetIsReceivingMouseEvents(const bool receiveMouseEvents)
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

    bool UiMouseBindings::IsMouseMoveEventDirty() const
    {
        return bMouseMoveEventDirty;
    }

    glm::ivec4 UiMouseBindings::FlushMouseMoveEvent()
    {
        glm::ivec4 result = glm::ivec4(0);

        if (bMouseMoveEventDirty)
        {
            result = mLastMouseMoveEvent;
            bMouseMoveEventDirty = false;
        }

        return result;
    }

    glm::ivec4 UiMouseBindings::GetLastMouseCursorPosition() const
    {
        return mLastMouseMoveEvent;
    }

    void UiMouseBindings::PushMouseMoveEvent(const glm::ivec4 &moveEvent)
    {
        mLastMouseMoveEvent = moveEvent;
        bMouseMoveEventDirty = true;
    }

    bool UiMouseBindings::IsMouseScrollEventDirty() const
    {
        return bMouseScrollEventDirty;
    }

    eMouseScrollDirection UiMouseBindings::FlushMouseScrollEvent()
    {
        eMouseScrollDirection result = eMouseScrollDirection::Undefined;

        if (bMouseScrollEventDirty)
        {
            result = mLastMouseScrollDirectionEvent;
            bMouseScrollEventDirty = false;
        }

        return result;
    }

    void UiMouseBindings::PushMouseScrollEvent(const eMouseScrollDirection mouseScrollEvent)
    {
        mLastMouseScrollDirectionEvent = mouseScrollEvent;
        bMouseScrollEventDirty = true;
    }

    void UiMouseBindings::ClearMouseScrollCache()
    {
        mLastMouseScrollDirectionEvent = eMouseScrollDirection::Undefined;
        bMouseScrollEventDirty = false;
    }

    void UiMouseBindings::ClearMouseMoveCache()
    {
        mLastMouseMoveEvent = {};
        bMouseMoveEventDirty = false;
    }
}
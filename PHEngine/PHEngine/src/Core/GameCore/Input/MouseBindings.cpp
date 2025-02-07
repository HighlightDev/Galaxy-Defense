#include "MouseBindings.h"

#include <iostream>

namespace EngineCore {

MouseBindings::MouseBindings()
    : mLastMouseMoveEvent(0)
    , bMouseMoveEventDirty(false)
    , mLastMouseScrollDirectionEvent(eMouseScrollDirection::Undefined)
    , bMouseScrollEventDirty(false)
    , mMouseKeysMaskVec()
    , mPressedMouseKeysCount(0)
{
    mMouseKeysMaskVec.reserve(3);
}

MouseBindings::~MouseBindings()
{
    UnsubscribeFromEvents();
}

void MouseBindings::UnsubscribeFromEvents()
{
    MouseMovedGameThreadEvent::GetInstance()->RemoveListener(MouseMovedGameThreadEvent::GetInstanceId());
    MouseScrollGameThreadEvent::GetInstance()->RemoveListener(MouseScrollGameThreadEvent::GetInstanceId());
    MouseButtonDownGameThreadEvent::GetInstance()->RemoveListener(MouseButtonDownGameThreadEvent::GetInstanceId());
}

void MouseBindings::SubscribeOnEvents()
{
    MouseMovedGameThreadEvent::GetInstance()->AddListener(shared_from_this());
    MouseScrollGameThreadEvent::GetInstance()->AddListener(shared_from_this());
    MouseButtonDownGameThreadEvent::GetInstance()->AddListener(shared_from_this());
}

void MouseBindings::Initialize()
{
    SubscribeOnEvents();
}

void MouseBindings::ProcessEvent(
    const MouseMovedGameThreadEvent* sender, const typename MouseMovedGameThreadEvent::EventData_t& mouseData)
{
    const glm::ivec4& mouseMoveData = std::get<0>(mouseData);
    PushMouseMoveEvent(mouseMoveData);
}

void MouseBindings::ProcessEvent(
    const MouseScrollGameThreadEvent* sender, const typename MouseScrollGameThreadEvent::EventData_t& mouseData)
{
    const eMouseScrollDirection mouseScrollDirection = std::get<0>(mouseData);
    PushMouseScrollEvent(mouseScrollDirection);
}

void MouseBindings::ProcessEvent(
    const MouseButtonDownGameThreadEvent* sender, const typename MouseButtonDownGameThreadEvent::EventData_t& data)
{
    if (eMouseEventTargetReceiverType::SCENE_GAME_OBJECTS == std::get<0>(data)) {
        mMouseKeysMaskVec = std::move(std::get<1>(data));
    }
}

KeyState MouseBindings::GetKeyState(const eMouseKeys mouseButtonKey) const
{
    KeyState state = KeyState::RELEASED;

    auto it = std::find_if(mMouseKeysMaskVec.begin(), mMouseKeysMaskVec.end(), [=](const auto& keyData) -> bool {
        return keyData.Key == mouseButtonKey;
    });

    if (it != mMouseKeysMaskVec.end()) {
        state = it->State;
    }

    return state;
}

void MouseBindings::SetIsReceivingMouseEvents(const bool receiveMouseEvents)
{
    if (bReceiveMouseEvents != receiveMouseEvents) {
        bReceiveMouseEvents = receiveMouseEvents;
        if (bReceiveMouseEvents) {
            SubscribeOnEvents();
        } else {
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

    if (bMouseMoveEventDirty) {
        result = mLastMouseMoveEvent;
        bMouseMoveEventDirty = false;
    }

    return result;
}

glm::ivec4 MouseBindings::GetLastMouseCursorPosition() const
{
    return mLastMouseMoveEvent;
}

void MouseBindings::PushMouseMoveEvent(const glm::ivec4& moveEvent)
{
    mLastMouseMoveEvent = moveEvent;
    bMouseMoveEventReceivedAtLeastOnce = true;
    bMouseMoveEventDirty = true;
}

bool MouseBindings::GetMouseMoveEventReceivedAtLeastOnce() const
{
    return bMouseMoveEventReceivedAtLeastOnce;
}

bool MouseBindings::IsMouseScrollEventDirty() const
{
    return bMouseScrollEventDirty;
}

eMouseScrollDirection MouseBindings::FlushMouseScrollEvent()
{
    eMouseScrollDirection result = eMouseScrollDirection::Undefined;

    if (bMouseScrollEventDirty) {
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
} // namespace EngineCore
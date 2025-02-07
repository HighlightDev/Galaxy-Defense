#pragma once

#include "Core/GameCore/Event/MouseButtonDownEvent.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"

#include <glm/vec4.hpp>

#include <memory>
#include <vector>

using namespace Event;

namespace EngineCore {
class UiMouseBindings : public MouseMovedGameThreadEvent,
                        public MouseScrollGameThreadEvent,
                        public MouseButtonDownGameThreadEvent,
                        public std::enable_shared_from_this<UiMouseBindings> {
    glm::ivec4 mLastMouseMoveEvent;

    bool bMouseMoveEventDirty;

    eMouseScrollDirection mLastMouseScrollDirectionEvent;

    bool bMouseScrollEventDirty;

    std::vector<MouseKeysData> mMouseKeysMaskVec;

    size_t mPressedMouseKeysCount;

    bool bReceiveMouseEvents{true};

public:
    UiMouseBindings();

    virtual ~UiMouseBindings();

    void Initialize();

    void
    ProcessEvent(const MouseMovedGameThreadEvent* sender, const typename MouseMovedGameThreadEvent::EventData_t& data) override;

    void
    ProcessEvent(const MouseScrollGameThreadEvent* sender, const typename MouseScrollGameThreadEvent::EventData_t& data) override;

    void ProcessEvent(
        const MouseButtonDownGameThreadEvent* sender, const typename MouseButtonDownGameThreadEvent::EventData_t& data) override;

    bool IsMouseMoveEventDirty() const;

    bool IsMouseScrollEventDirty() const;

    eMouseScrollDirection FlushMouseScrollEvent();

    glm::ivec4 FlushMouseMoveEvent();

    glm::ivec4 GetLastMouseCursorPosition() const;

    void ClearMouseScrollCache();

    void ClearMouseMoveCache();

    KeyState GetKeyState(const eMouseKeys mouseButtonKey) const;

    void SetIsReceivingMouseEvents(const bool receiveMouseEvents);

protected:
    void PushMouseMoveEvent(const glm::ivec4& moveEvent);

    void PushMouseScrollEvent(const eMouseScrollDirection mouseScrollEvent);

private:
    void UnsubscribeFromEvents();

    void SubscribeOnEvents();
};

}; // namespace EngineCore

#include "InputManager.h"

#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Event/MouseButtonDownEvent.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"

using namespace Event;

namespace EngineCore {
InputManager::InputManager()
    : mPrevMouseX(0)
    , mPrevMouseY(0)
    , mKeyboardMaskVec()
    , mMouseButtonMaskVec()
{
}

void InputManager::TriggerOnKeyboardKeyDown(eKeyboardKeys key)
{
    SetKeyboardKeyState(KeyboardKeysData(key, KeyState::PRESSED));
}

void InputManager::TriggerOnKeyboardKeyUp(eKeyboardKeys key)
{
    SetKeyboardKeyState(KeyboardKeysData(key, KeyState::RELEASED));
}

void InputManager::SetKeyboardKeyState(const KeyboardKeysData key)
{
    auto it = std::find_if(
        mKeyboardMaskVec.begin(), mKeyboardMaskVec.end(), [=](const auto& keyData) -> bool { return keyData.Key == key.Key; });

    if (it == mKeyboardMaskVec.end()) {
        mKeyboardMaskVec.emplace_back(key);
    } else {
        it->State = key.State;
    }

    KeyboardButtonDownGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, mKeyboardMaskVec);
    KeyboardButtonDownLuaThreadEvent::GetInstance()->SendEvent(eExecutionOrder::PRE_EXECUTION, mKeyboardMaskVec);
}

void InputManager::TriggerOnMouseMove(const int32_t x, const int32_t y)
{
    const int32_t deltaMouseX = x - mPrevMouseX;
    const int32_t deltaMouseY = y - mPrevMouseY;

    mPrevMouseX = x;
    mPrevMouseY = y;

    MouseMovedGameThreadEvent::GetInstance()->SendEvent(
        Event::eExecutionOrder::PRE_EXECUTION, glm::ivec4(x, y, deltaMouseX, deltaMouseY));
    MouseMovedLuaThreadEvent::GetInstance()->SendEvent(
        Event::eExecutionOrder::PRE_EXECUTION, glm::ivec4(x, y, deltaMouseX, deltaMouseY));
}

void InputManager::TriggeOnMouseScroll(const eMouseScrollDirection scrollDirection, const float scrollOffset)
{
    MouseScrollGameThreadEvent::GetInstance()->SendEvent(Event::eExecutionOrder::PRE_EXECUTION, scrollDirection, scrollOffset);
    MouseScrollLuaThreadEvent::GetInstance()->SendEvent(Event::eExecutionOrder::PRE_EXECUTION, scrollDirection, scrollOffset);
}

void InputManager::TriggerOnMouseButtonKeyDown(const eMouseKeys key)
{
    SetMouseButtonKeyState(MouseKeysData(key, KeyState::PRESSED));
}

void InputManager::TriggerOnMouseButtonKeyUp(const eMouseKeys key)
{
    SetMouseButtonKeyState(MouseKeysData(key, KeyState::RELEASED));
}

void InputManager::SetMouseButtonKeyState(const MouseKeysData key)
{
    auto it = std::find_if(mMouseButtonMaskVec.begin(), mMouseButtonMaskVec.end(), [=](const auto& keyData) -> bool {
        return keyData.Key == key.Key;
    });

    if (it == mMouseButtonMaskVec.end()) {
        mMouseButtonMaskVec.emplace_back(key);
    } else {
        it->State = key.State;
    }

    MouseButtonDownRootEvent::GetInstance()->SendEvent(
        eExecutionOrder::PRE_EXECUTION, glm::ivec2(mPrevMouseX, mPrevMouseY), mMouseButtonMaskVec);

    mMouseButtonMaskVec.clear();
}

} // namespace EngineCore

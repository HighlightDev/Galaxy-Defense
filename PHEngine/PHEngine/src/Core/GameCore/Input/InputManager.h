#pragma once

#include "Core/GameCore/Input/InputDeviceKeyData.h"
#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

#include <stdint.h>

#include <cstddef>
#include <vector>

namespace EngineCore {
class InputManager {

    int32_t mPrevMouseX;
    int32_t mPrevMouseY;

    std::vector<KeyboardKeysData> mKeyboardMaskVec;
    std::vector<MouseKeysData> mMouseButtonMaskVec;

public:
    InputManager();

    ~InputManager() = default;

    // KEYBOARD
    void TriggerOnKeyboardKeyDown(eKeyboardKeys key);

    void TriggerOnKeyboardKeyUp(eKeyboardKeys key);

    // MOUSE
    void TriggerOnMouseMove(const int32_t x, const int32_t y);

    void TriggeOnMouseScroll(const eMouseScrollDirection scrollDirection);

    void TriggerOnMouseButtonKeyDown(const eMouseKeys key);

    void TriggerOnMouseButtonKeyUp(const eMouseKeys key);

private:
    void SetKeyboardKeyState(const KeyboardKeysData key);

    void SetMouseButtonKeyState(const MouseKeysData key);
};

} // namespace EngineCore

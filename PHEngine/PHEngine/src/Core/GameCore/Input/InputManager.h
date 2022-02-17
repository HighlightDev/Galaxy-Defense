#pragma once

#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/Input/KeyboardData.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

#include <vector>
#include <cstddef>
#include <stdint.h>

namespace Game
{
   class InputManager
   {

      int32_t mPrevMouseX;
      int32_t mPrevMouseY;

      std::vector<KeyboardData> mKeyboardMaskVec;

   public:
      InputManager();

      ~InputManager() = default;

      // KEYBOARD
      void TriggerOnKeyDown(Keys key);

      void TriggerOnKeyUp(Keys key);

      // MOUSE
      void TriggerOnMouseMove(const int32_t x, const int32_t y);

      void TriggeOnMouseScroll(const eMouseScrollDirection scrollDirection);

   private:
      void SetKeyState(KeyboardData key);
   };

}

#pragma once

#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

#include <cstddef>
#include <stdint.h>

namespace Game
{
   class InputManager
   {

      int32_t mMouseX = 0, mMouseY = 0;

   public:

      InputManager() = default;

      ~InputManager() = default;

      /*KEYBOARD*/
      void TriggerOnKeyDown(Keys key);

      void TriggerOnKeyUp(Keys key);

      /*MOUSE*/
      void TriggerOnMouseMove(const int32_t x, const int32_t y);

      void TriggeOnMouseScroll(const eMouseScrollDirection scrollDirection);

   };

}


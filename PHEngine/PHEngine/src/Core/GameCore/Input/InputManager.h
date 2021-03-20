#pragma once

#include "Core/GameCore/Input/Keys.h"

namespace Game
{
   class InputManager
   {

   public:

      InputManager() = default;

      ~InputManager() = default;

      /*KEYBOARD*/
      void TriggerOnKeyDown(Keys key);

      void TriggerOnKeyUp(Keys key);

      /*MOUSE*/
      void TriggerOnMouseMove();

   };

}


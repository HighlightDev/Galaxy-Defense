#pragma once

#include "Keys.h"

namespace Game {

   struct KeyboardData
   {
      Keys Key;
      KeyState State;

      KeyboardData(Keys key, KeyState state)
         : Key(key)
         , State(state)
      {

      }

      KeyboardData()
         : Key(Keys::None)
         , State(KeyState::RELEASED)
      {
      }
   };
}
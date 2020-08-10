#pragma once

#include <unordered_map>

#include "Keys.h"

namespace Game
{

   class KeyboardBindings
   {
      std::unordered_map<Keys, KeyState> keyboardMaskMap;

      size_t mPressedKeysCount = 0;

   public:

      KeyboardBindings();

      ~KeyboardBindings();

      bool HasPressedKeys() const;

      void KeyPress(Keys key);

      void KeyRelease(Keys key);

      KeyState GetKeyState(Keys key) const;
   };

};


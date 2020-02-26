#pragma once

#include <unordered_map>

#include "Keys.h"

namespace Game
{

   class KeyboardBindings
   {
      std::unordered_map<Keys, bool> keyboardMaskMap;

   public:

      KeyboardBindings();

      ~KeyboardBindings();

      bool HasPressedKeys() const;

      void AllocateKey(Keys key);

      void KeyPress(Keys key);

      void KeyRelease(Keys key);

      bool GetKeyState(Keys key) const;
   };

};


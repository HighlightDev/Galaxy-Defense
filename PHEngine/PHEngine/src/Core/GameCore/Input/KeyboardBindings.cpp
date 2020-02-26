#include "KeyboardBindings.h"

#include <algorithm>

namespace Game
{

   KeyboardBindings::KeyboardBindings()
   {
   }


   KeyboardBindings::~KeyboardBindings()
   {
   }

   bool KeyboardBindings::HasPressedKeys() const
   {
      auto pressedItem = std::find_if(keyboardMaskMap.begin(), keyboardMaskMap.end(), [](const std::pair<Keys, bool>& pair) { return pair.second; });
      return pressedItem != keyboardMaskMap.end();
   }

   void KeyboardBindings::AllocateKey(Keys key)
   {
      keyboardMaskMap.emplace(std::make_pair(key, true));
   }

   void  KeyboardBindings::KeyPress(Keys key)
   {
      auto it = keyboardMaskMap.find(key);
      if (it == keyboardMaskMap.end())
         AllocateKey(key);
   }

   void KeyboardBindings::KeyRelease(Keys key)
   {
      auto it = keyboardMaskMap.find(key);
      if (it != keyboardMaskMap.end())
         keyboardMaskMap.erase(it);
   }

   bool KeyboardBindings::GetKeyState(Keys key) const
   {
      return keyboardMaskMap.find(key) != keyboardMaskMap.end();
   }

}
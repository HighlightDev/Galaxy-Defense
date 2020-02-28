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
      return mPressedKeysCount > 0;
   }

   void KeyboardBindings::AllocateKey(Keys key)
   {
      keyboardMaskMap.emplace(std::make_pair(key, true));
      mPressedKeysCount++;
   }

   void  KeyboardBindings::KeyPress(Keys key)
   {
      auto it = keyboardMaskMap.find(key);
      if (it == keyboardMaskMap.end())
      {
         AllocateKey(key);
      }
      else
      {
         if (!it->second)
         {
            it->second = true;
            mPressedKeysCount++;
         }
      }
   }

   void KeyboardBindings::KeyRelease(Keys key)
   {
      auto it = keyboardMaskMap.find(key);
      if (it != keyboardMaskMap.end())
      {
         it->second = false;
         mPressedKeysCount--;
      }
   }

   bool KeyboardBindings::GetKeyState(Keys key) const
   {
      const auto& it = keyboardMaskMap.find(key);
      return  it != keyboardMaskMap.end() && it->second;
   }

}
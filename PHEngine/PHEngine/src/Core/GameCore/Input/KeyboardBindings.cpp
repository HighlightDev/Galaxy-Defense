#include "KeyboardBindings.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"

#include <algorithm>

using namespace Event;

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

   void  KeyboardBindings::KeyPress(Keys key)
   {
      if (auto it = keyboardMaskMap.find(key); it == keyboardMaskMap.end())
      {
         keyboardMaskMap.emplace(std::make_pair(key, KeyState::PRESSED));
      }
      else 
      {
         it->second = KeyState::PRESSED;
      }

      mPressedKeysCount++;

      KeyboardEventData data(key, KeyState::PRESSED);
      KeyboardButtonDownEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, data);
   }

   void KeyboardBindings::KeyRelease(Keys key)
   {
      if (auto it = keyboardMaskMap.find(key); it != keyboardMaskMap.end())
      {
         it->second = KeyState::RELEASED;
         mPressedKeysCount--;
      }

      KeyboardEventData data(key, KeyState::RELEASED);
      KeyboardButtonDownEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, data);
   }

   KeyState KeyboardBindings::GetKeyState(Keys key) const
   {
      KeyState result = KeyState::RELEASED;

      if (const auto& it = keyboardMaskMap.find(key); it != keyboardMaskMap.end())
         result = it->second;

      return result;
   }

}
#include "InputManager.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"

using namespace Event;

namespace Game
{
   InputManager::InputManager()
       : mPrevMouseX(0), mPrevMouseY(0), mKeyboardMaskVec()
   {
   }

   void InputManager::TriggerOnKeyDown(Keys key)
   {
      SetKeyState(KeyboardData(key, KeyState::PRESSED));
   }

   void InputManager::TriggerOnKeyUp(Keys key)
   {
      SetKeyState(KeyboardData(key, KeyState::RELEASED));
   }

   void InputManager::SetKeyState(KeyboardData key)
   {
      auto it = std::find_if(mKeyboardMaskVec.begin(),
                             mKeyboardMaskVec.end(), [=](const auto &keyData) -> bool
                             { return keyData.Key == key.Key; });

      if (it == mKeyboardMaskVec.end())
      {
         mKeyboardMaskVec.emplace_back(key);
      }
      else
      {
         it->State = key.State;
      }

      KeyboardButtonDownEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, mKeyboardMaskVec);
   }

   void InputManager::TriggerOnMouseMove(const int32_t x, const int32_t y)
   {
      const int32_t deltaMouseX = x - mPrevMouseX;
      const int32_t deltaMouseY = y - mPrevMouseY;

      mPrevMouseX = x;
      mPrevMouseY = y;

      MouseMovedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::PRE_EXECUTION, glm::ivec4(x, y, deltaMouseX, deltaMouseY));
   }

   void InputManager::TriggeOnMouseScroll(const eMouseScrollDirection scrollDirection)
   {
      MouseScrollEvent::GetInstance()->SendEvent(Event::ExecutionOrder::PRE_EXECUTION, scrollDirection);
   }

}

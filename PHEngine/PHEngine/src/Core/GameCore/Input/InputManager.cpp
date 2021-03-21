#include "InputManager.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Event/MouseMovedEvent.h"

using namespace Event;

namespace Game
{
   void InputManager::TriggerOnKeyDown(Keys key)
   {
      KeyboardData data{ key, KeyState::PRESSED };
      KeyboardButtonDownEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, data);
   }

   void InputManager::TriggerOnKeyUp(Keys key)
   {
      KeyboardData data{ key, KeyState::RELEASED };
      KeyboardButtonDownEvent::GetInstance()->SendEvent(ExecutionOrder::PRE_EXECUTION, data);
   }

   void InputManager::TriggerOnMouseMove(const int32_t x, const int32_t y)
   {
      const int32_t deltaMouseX = x - mMouseX;
      const int32_t deltaMouseY = y - mMouseY;

      mMouseX = x;
      mMouseY = y;

      MouseMovedEvent::GetInstance()->SendEvent(Event::ExecutionOrder::PRE_EXECUTION, glm::ivec4(x, y, deltaMouseX, deltaMouseY));
   }
}

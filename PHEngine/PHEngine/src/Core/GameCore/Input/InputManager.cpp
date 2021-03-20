#include "InputManager.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"

using namespace Event;

namespace Game
{
   void InputManager::TriggerOnKeyDown(Keys key)
   {
      KeyboardData data{ key, KeyState::PRESSED };
      KeyboardButtonDownEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, data);
   }

   void InputManager::TriggerOnKeyUp(Keys key)
   {
      KeyboardData data{ key, KeyState::RELEASED };
      KeyboardButtonDownEvent::GetInstance()->SendEvent(ExecutionOrder::POST_EXECUTION, data);
   }

   void InputManager::TriggerOnMouseMove()
   {
      //todo: later
   }
}

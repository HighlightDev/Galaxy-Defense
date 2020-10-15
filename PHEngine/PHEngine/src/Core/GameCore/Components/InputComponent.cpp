#include "InputComponent.h"
#include "Core/GameCore/KeyboardInputManager.h"

namespace Game
{

   InputComponent::InputComponent(const std::string& gameObjectName)
      : Component(gameObjectName)
   {
   }

   InputComponent::~InputComponent()
   {
   }

   ComponentType InputComponent::GetComponentType() const
   {
      return INPUT_COMPONENT;
   }

   const KeyboardBindings& InputComponent::GetKeyboardBindings() const
   {
      return m_keyboardBindings;
   }

   // Game thread tick
   void InputComponent::Tick(const float deltaTime)
   {
      if (KeyboardInputManager::GetInstance()->HasKeyEvents())
      {
         KeyboardInputManager::queue_t& keyEventsQueue = KeyboardInputManager::GetInstance()->GetKeyboardKeyEvents();

         while (keyEventsQueue.size() > 0)
         {
            std::pair<Keys, size_t>& pair = keyEventsQueue.front();
            keyEventsQueue.pop();
            if (pair.second == KEY_PRESSED)
            {
               m_keyboardBindings.KeyPress(pair.first);
            }
            else
            {
               m_keyboardBindings.KeyRelease(pair.first);
            }
         }
      }
   }

}
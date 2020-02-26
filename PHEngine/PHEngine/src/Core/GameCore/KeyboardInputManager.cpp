#include "KeyboardInputManager.h"

#include <utility>

namespace Game
{

   KeyboardInputManager* KeyboardInputManager::m_instance = nullptr;

   KeyboardInputManager::KeyboardInputManager()
   {
   }

   KeyboardInputManager::~KeyboardInputManager()
   {
      delete m_instance;
   }

   bool KeyboardInputManager::HasKeyEvents() const
   {
      return m_keyboardKeyEvents.size() > 0;
   }

   void KeyboardInputManager::PushKeyEvent(Keys key, size_t KeyState)
   {
      m_keyboardKeyEvents.push(std::make_pair<Keys, size_t>(std::move(key), std::move(KeyState)));
   }

   KeyboardInputManager::queue_t& KeyboardInputManager::GetKeyboardKeyEvents()
   {
      return m_keyboardKeyEvents;
   }

}

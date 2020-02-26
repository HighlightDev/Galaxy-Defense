#pragma once

#include <queue>

#include "Core/GameCore/Input/Keys.h"

namespace Game
{

#define KEY_PRESSED 1
#define KEY_RELEASED 0

   class KeyboardInputManager
   {
      
   public:

      using queue_t = std::queue<std::pair<Keys, size_t>>;

   private:

      static KeyboardInputManager* m_instance;

      queue_t m_keyboardKeyEvents;

      KeyboardInputManager();

   public:

      ~KeyboardInputManager();

      void PushKeyEvent(Keys key, size_t KeyState);

      queue_t& GetKeyboardKeyEvents();

      bool HasKeyEvents() const;

      static KeyboardInputManager* GetInstance()
      {
         if (!m_instance)
            m_instance = new KeyboardInputManager();

         return m_instance;
      }
   };

}


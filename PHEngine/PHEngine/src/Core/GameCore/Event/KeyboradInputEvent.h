#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/Keys.h"

using namespace Game;

namespace Event
{
   struct KeyboardEventData
   {
      Keys Key;
      KeyState State;

      KeyboardEventData(Keys key, KeyState state)
         : Key(key)
         , State(state)
      {

      }

      KeyboardEventData()
         : Key(Keys::None)
         , State(KeyState::RELEASED)
      {
      }
   };

   struct KeyboardInputEvent
      : public TEvent<AtomicEventPolicy<KeyboardEventData>>
   {
   public:
      using Event_t = TEvent<AtomicEventPolicy<KeyboardEventData>>::Event_t;
   };

}

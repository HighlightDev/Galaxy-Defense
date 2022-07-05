#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/Keys.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"

#include <vector>

using namespace EngineCore;

namespace Event
{
   struct KeyboardButtonDownEvent
      : public TEvent<SingleDataEventPolicy<std::vector<KeyboardKeysData>>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<std::vector<KeyboardKeysData>>>::Event_t;

      virtual std::string ToString() const override {
         return "KeyboardButtonDownEvent";
      }
   };

}

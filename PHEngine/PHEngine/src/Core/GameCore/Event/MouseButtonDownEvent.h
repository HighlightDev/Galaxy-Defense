#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"

#include <vector>

using namespace EngineCore;

namespace Event
{
   struct MouseButtonDownEvent
      : public TEvent<SingleDataEventPolicy<std::vector<MouseKeysData>>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<std::vector<MouseKeysData>>>::Event_t;

      std::string ToString() const override {
         return "MouseButtonDownEvent";
      }
   };

}

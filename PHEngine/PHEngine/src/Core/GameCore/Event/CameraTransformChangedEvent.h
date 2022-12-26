#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/ACamera.h"

using namespace EngineCore;

namespace Event
{

   class CameraTransformChangedEvent
      : public TEvent<SingleDataEventPolicy<ACamera*>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<ACamera*>>::Event_t;

      std::string ToString() const override {
         return "CameraTransformChangedEvent";
      }
   };

}
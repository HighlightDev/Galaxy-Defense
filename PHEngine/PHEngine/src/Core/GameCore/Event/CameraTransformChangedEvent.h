#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/ICamera.h"

using namespace Game;

namespace Event
{

   class CameraTransformChangedEvent
      : public TEvent<AtomicEventPolicy<ICamera*>>
   {
   public:
      using Event_t = TEvent<AtomicEventPolicy<ICamera*>>::Event_t;
   };

}
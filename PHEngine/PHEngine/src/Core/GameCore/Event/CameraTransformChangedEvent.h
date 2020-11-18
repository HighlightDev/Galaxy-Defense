#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/ACamera.h"

using namespace Game;

namespace Event
{

   class CameraTransformChangedEvent
      : public TEvent<AtomicEventPolicy<ACamera*>>
   {
   public:
      using Event_t = TEvent<AtomicEventPolicy<ACamera*>>::Event_t;
   };

}
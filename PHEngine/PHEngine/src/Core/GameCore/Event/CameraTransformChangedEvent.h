#pragma once

#include "TEvent.h"
#include "Core/GameCore/ICamera.h"

using namespace Game;

namespace Event
{

   class CameraTransformChangedEvent
      : public TEvent<ICamera*>
   {
   public:
      using Event_t = TEvent<ICamera*>::Event_t;
   };

}
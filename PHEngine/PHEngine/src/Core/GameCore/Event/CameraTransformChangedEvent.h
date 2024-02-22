#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/ACamera.h"

using namespace EngineCore;

namespace Event
{

   class CameraTransformChangedGameThreadEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<ACamera*>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<ACamera*>>::Event_t;

      std::string ToString() const override {
         return "GameThreadCameraTransformChangedEvent";
      }
   };

}
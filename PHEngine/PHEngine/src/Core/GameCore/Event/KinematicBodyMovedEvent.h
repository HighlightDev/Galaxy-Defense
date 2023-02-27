#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

namespace Event
{

   class KinematicBodyMovedEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<EnginePhysics::PhysicsDescriptor*, EngineCore::EulerAnglesTransform>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<EnginePhysics::PhysicsDescriptor*, EngineCore::EulerAnglesTransform>>::Event_t;

      std::string ToString() const override {
         return "GameThreadKinematicBodyMovedEvent";
      }
   };

}
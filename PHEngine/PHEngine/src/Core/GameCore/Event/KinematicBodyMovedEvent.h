#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

namespace Event
{

   class KinematicBodyMovedEvent
      : public TEvent<SingleDataEventPolicy<EnginePhysics::PhysicsDescriptor*, EngineCore::EulerAnglesTransform>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<EnginePhysics::PhysicsDescriptor*, EngineCore::EulerAnglesTransform>>::Event_t;

      virtual std::string ToString() const override {
         return "KinematicBodyMovedEvent";
      }
   };

}
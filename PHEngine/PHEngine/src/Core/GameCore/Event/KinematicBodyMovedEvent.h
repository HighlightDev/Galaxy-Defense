#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

#include <memory>

namespace Event
{

   class KinematicBodyMovedGameThreadEvent
      : public TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::weak_ptr<EnginePhysics::PhysicsDescriptor>, EngineCore::EulerAnglesTransform>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::weak_ptr<EnginePhysics::PhysicsDescriptor>, EngineCore::EulerAnglesTransform>>::Event_t;

      std::string ToString() const override {
         return "KinematicBodyMovedGameThreadEvent";
      }
   };

}
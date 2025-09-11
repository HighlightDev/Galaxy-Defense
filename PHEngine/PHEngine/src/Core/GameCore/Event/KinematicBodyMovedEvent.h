#pragma once

#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Policy/Policies.h"
#include "TEvent.h"

#include <memory>

namespace Event {

class KinematicBodyMovedGameThreadEvent
    : public TEvent<
          KinematicBodyMovedGameThreadEvent,
          eEventThreadType::GAME_THREAD,
          SingleDataEventPolicy<std::weak_ptr<EnginePhysics::PhysicsDescriptor>, EngineCore::EulerAnglesTransform>> {
public:
    using Type_t = TEvent<
        KinematicBodyMovedGameThreadEvent,
        eEventThreadType::GAME_THREAD,
        SingleDataEventPolicy<std::weak_ptr<EnginePhysics::PhysicsDescriptor>, EngineCore::EulerAnglesTransform>>::Type_t;

    std::string ToString() const override
    {
        return "KinematicBodyMovedGameThreadEvent";
    }
};

} // namespace Event
#pragma once

#include "Policy/Policies.h"
#include "TEvent.h"

#include <string>

namespace Event {
class PhysicsComponentUpdatedGameThreadEvent
    : public TEvent<PhysicsComponentUpdatedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::string>> {
public:
    using Event
        = TEvent<PhysicsComponentUpdatedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::string>>::
            Type_t;

    std::string ToString() const override
    {
        return "PhysicsComponentUpdatedGameThreadEvent";
    }
};

} // namespace Event
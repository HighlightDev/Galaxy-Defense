#pragma once

#include "Core/GameCore/Event/Policy/Policies.h"
#include "Core/GameCore/Event/TEvent.h"

#include <stdint.h>

#include <cstdint>
#include <vector>

namespace Event {
class ElectroRaySphereContactCollisionEvent
    : public TEvent<
          ElectroRaySphereContactCollisionEvent,
          eEventThreadType::GAME_THREAD,
          MultipleDataEventPolicy<int32_t /*srcCollisionObject*/, std::vector<int32_t>> /*dstCollisionObjects*/> {
public:
    using Type_t = TEvent<
        ElectroRaySphereContactCollisionEvent,
        eEventThreadType::GAME_THREAD,
        MultipleDataEventPolicy<int32_t, std::vector<int32_t>>>::Type_t;

    std::string ToString() const override
    {
        return "GameThreadElectroRaySphereContactCollisionEvent";
    }
};

} // namespace Event
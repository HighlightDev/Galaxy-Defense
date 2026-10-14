#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event {

struct PhysicsDescriptorRemovedGameThreadEvent
    : public TEvent<PhysicsDescriptorRemovedGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<size_t>> {
public:
    using Type_t
        = TEvent<PhysicsDescriptorRemovedGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<size_t>>::Type_t;

    std::string ToString() const override
    {
        return "PhysicsDescriptorRemovedGameThreadEvent";
    }
};

} // namespace Event
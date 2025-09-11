#pragma once

#include "Core/GameCore/Event/TEvent.h"

namespace Event {
struct PauseGameThreadEvent : public TEvent<PauseGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<bool>> {
public:
    using Type_t = TEvent<PauseGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<bool>>::Type_t;

    std::string ToString() const override
    {
        return "PauseGameThreadEvent";
    }
};

} // namespace Event
